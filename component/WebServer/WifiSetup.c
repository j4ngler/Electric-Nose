#include "WifiSetup.h"
#include "FileServer.h"

#include <inttypes.h>
#include <stdlib.h>
#include "cJSON.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "nvs.h"

#define SETUP_SSID "E-Nose-Setup"
#define CONNECT_TIMEOUT_US (20 * 1000 * 1000)
#define RETRY_DELAY_US (2 * 1000 * 1000)
#define SCAN_LIMIT 20

typedef struct {
    char ssid[33];
    char password[65];
} wifi_credentials_t;

typedef enum {
    IDLE, WAIT_DISCONNECT, CONNECTING, CONNECTED, FAILED
} wifi_state_t;

static const char *TAG = "wifi_setup";
static SemaphoreHandle_t lock;
static esp_netif_t *ap_netif;
static nvs_handle_t storage;
static bool storage_ready;
static bool ap_enabled;
static bool save_on_connect;
static bool saved_to_flash;
static bool scanning;
static int64_t deadline;
static int64_t retry_at;
static int64_t close_ap_at;
static wifi_credentials_t active;
static wifi_config_t ap_config;
static wifi_state_t state;
static esp_ip4_addr_t sta_ip;
static unsigned disconnect_reason;
static esp_err_t (*show_file_index)(httpd_req_t *);

/* All state transitions run with lock held. */
static esp_err_t enable_setup(void)
{
    if (ap_enabled) return ESP_OK;
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_APSTA), TAG, "enable setup AP");
    ap_enabled = true;
    ESP_LOGI(TAG, "%s password: %s; http://192.168.4.1", SETUP_SSID, ap_config.ap.password);
    return ESP_OK;
}

static void connection_failed(void)
{
    state = FAILED;
    retry_at = 0;
    sta_ip.addr = 0;
    save_on_connect = false;
    memset(&active, 0, sizeof(active));
    esp_wifi_disconnect();
    enable_setup();
}

static void connect_active(void)
{
    wifi_config_t config = {0};
    memcpy(config.sta.ssid, active.ssid, strnlen(active.ssid, sizeof(config.sta.ssid)));
    memcpy(config.sta.password, active.password,
           strnlen(active.password, sizeof(config.sta.password)));
    config.sta.threshold.authmode = active.password[0] ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
    config.sta.pmf_cfg.capable = true;
    state = CONNECTING;
    if (esp_wifi_set_config(WIFI_IF_STA, &config) != ESP_OK ||
        esp_wifi_connect() != ESP_OK) {
        connection_failed();
    }
}

static void begin_connection(const wifi_credentials_t *next, bool save)
{
    bool was_connected = state == CONNECTED;
    active = *next;
    save_on_connect = save;
    saved_to_flash = !save;
    disconnect_reason = 0;
    retry_at = 0;
    sta_ip.addr = 0;
    deadline = esp_timer_get_time() + CONNECT_TIMEOUT_US;
    if (was_connected) {
        state = WAIT_DISCONNECT;
        if (esp_wifi_disconnect() == ESP_OK) return;
    }
    connect_active();
}

static void network_event(void *arg, esp_event_base_t base, int32_t event, void *data)
{
    xSemaphoreTake(lock, portMAX_DELAY);
    if (base == WIFI_EVENT) {
        const wifi_event_sta_disconnected_t *disconnected = data;
        if (state == WAIT_DISCONNECT) {
            connect_active();
        } else if (state == CONNECTED || state == CONNECTING) {
            if (state == CONNECTED) deadline = esp_timer_get_time() + CONNECT_TIMEOUT_US;
            state = CONNECTING;
            sta_ip.addr = 0;
            disconnect_reason = disconnected->reason;
            retry_at = esp_timer_get_time() + RETRY_DELAY_US;
        }
    } else if (state == CONNECTING || state == CONNECTED) {
        const ip_event_got_ip_t *got_ip = data;
        wifi_ap_record_t current;
        if (esp_wifi_sta_get_ap_info(&current) == ESP_OK &&
            strcmp((const char *)current.ssid, active.ssid) == 0) {
            state = CONNECTED;
            retry_at = 0;
            disconnect_reason = 0;
            sta_ip = got_ip->ip_info.ip;
            if (save_on_connect) {
                saved_to_flash = storage_ready &&
                    nvs_set_blob(storage, "credentials", &active, sizeof(active)) == ESP_OK &&
                    nvs_commit(storage) == ESP_OK;
                save_on_connect = false;
            }
            ESP_LOGI(TAG, "LAN address: http://" IPSTR, IP2STR(&sta_ip));
        }
    }
    xSemaphoreGive(lock);
}

static void manage_network(void *arg)
{
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(250));
        xSemaphoreTake(lock, portMAX_DELAY);
        const int64_t now = esp_timer_get_time();
        if ((state == CONNECTING || state == WAIT_DISCONNECT) && now >= deadline) {
            connection_failed();
        } else if (state == CONNECTING && retry_at && now >= retry_at && !scanning) {
            retry_at = 0;
            if (esp_wifi_connect() != ESP_OK) connection_failed();
        }
        if ((state == FAILED || state == IDLE) && !ap_enabled) enable_setup();
        if (close_ap_at && now >= close_ap_at) {
            close_ap_at = 0;
            if (state == CONNECTED && saved_to_flash && esp_wifi_set_mode(WIFI_MODE_STA) == ESP_OK) {
                ap_enabled = false;
            }
        }
        xSemaphoreGive(lock);
    }
}

static bool request_address(httpd_req_t *req, struct in_addr *ip)
{
    struct sockaddr_storage address;
    socklen_t length = sizeof(address);
    if (getsockname(httpd_req_to_sockfd(req), (struct sockaddr *)&address, &length) != 0) {
        return false;
    }
    if (address.ss_family == AF_INET) {
        const struct sockaddr_in *v4 = (const struct sockaddr_in *)&address;
        *ip = v4->sin_addr;
        return true;
    }
#if CONFIG_LWIP_IPV6
    if (address.ss_family == AF_INET6) {
        const struct sockaddr_in6 *v6 = (const struct sockaddr_in6 *)&address;
        static const uint8_t mapped_prefix[12] = { [10] = 0xff, [11] = 0xff };
        if (memcmp(v6->sin6_addr.s6_addr, mapped_prefix, sizeof(mapped_prefix)) == 0) {
            memcpy(ip, &v6->sin6_addr.s6_addr[12], sizeof(*ip));
            return true;
        }
    }
#endif
    return false;
}

static bool on_setup_network(httpd_req_t *req)
{
    struct in_addr address;
    esp_netif_ip_info_t ap;
    return ap_netif && esp_netif_get_ip_info(ap_netif, &ap) == ESP_OK &&
           request_address(req, &address) && address.s_addr == ap.ip.addr;
}

static esp_err_t send_json(httpd_req_t *req, cJSON *json)
{
    char *body = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    if (!body) return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    esp_err_t err = httpd_resp_sendstr(req, body);
    cJSON_free(body);
    return err;
}

static esp_err_t send_error(httpd_req_t *req, const char *status, const char *message)
{
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, message);
}

static esp_err_t show_setup(httpd_req_t *req)
{
    extern const char page_start[] asm("_binary_wifi_setup_html_start");
    extern const char page_end[] asm("_binary_wifi_setup_html_end");
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, page_start, page_end - page_start);
}

static esp_err_t show_index(httpd_req_t *req)
{
    if (!on_setup_network(req)) return show_file_index(req);
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/wifi");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t get_status(httpd_req_t *req)
{
    static const char *const names[] = {
        [IDLE] = "setup", [WAIT_DISCONNECT] = "connecting", [CONNECTING] = "connecting",
        [CONNECTED] = "connected", [FAILED] = "failed",
    };
    if (!lock) return send_error(req, "503 Service Unavailable", "Wi-Fi unavailable");
    cJSON *result = cJSON_CreateObject();
    xSemaphoreTake(lock, portMAX_DELAY);
    char ip[16] = "";
    if (sta_ip.addr) snprintf(ip, sizeof(ip), IPSTR, IP2STR(&sta_ip));
    cJSON_AddStringToObject(result, "state", names[state]);
    cJSON_AddStringToObject(result, "ip", ip);
    cJSON_AddBoolToObject(result, "setup", ap_enabled);
    cJSON_AddBoolToObject(result, "saved", state == CONNECTED && saved_to_flash);
    cJSON_AddNumberToObject(result, "reason", disconnect_reason);
    xSemaphoreGive(lock);
    cJSON_AddBoolToObject(result, "local", on_setup_network(req));
    return send_json(req, result);
}

static esp_err_t scan_networks(httpd_req_t *req)
{
    if (!on_setup_network(req)) {
        return httpd_resp_send_err(req, HTTPD_403_FORBIDDEN, "Join E-Nose-Setup first");
    }
    xSemaphoreTake(lock, portMAX_DELAY);
    bool busy = state == CONNECTING || state == WAIT_DISCONNECT || close_ap_at;
    scanning = !busy;
    xSemaphoreGive(lock);
    if (busy) return send_error(req, "409 Conflict", "Connection in progress");

    wifi_ap_record_t *records = calloc(SCAN_LIMIT, sizeof(*records));
    esp_err_t err = records ? esp_wifi_scan_start(NULL, true) : ESP_ERR_NO_MEM;
    uint16_t count = SCAN_LIMIT;
    if (err == ESP_OK) {
        err = esp_wifi_scan_get_ap_records(&count, records);
        if (err != ESP_OK) esp_wifi_clear_ap_list();
    }
    xSemaphoreTake(lock, portMAX_DELAY);
    scanning = false;
    xSemaphoreGive(lock);
    if (err != ESP_OK) {
        free(records);
        return send_error(req, "503 Service Unavailable", "Wi-Fi scan unavailable");
    }
    cJSON *result = cJSON_CreateArray();
    for (uint16_t i = 0; i < count; i++) {
        if (!records[i].ssid[0]) continue;
        cJSON *entry = cJSON_CreateObject();
        if (!cJSON_AddItemToArray(result, entry)) {
            cJSON_Delete(entry);
            break;
        }
        cJSON_AddStringToObject(entry, "ssid", (const char *)records[i].ssid);
        cJSON_AddNumberToObject(entry, "rssi", records[i].rssi);
        cJSON_AddBoolToObject(entry, "secure", records[i].authmode != WIFI_AUTH_OPEN);
    }
    free(records);
    return send_json(req, result);
}

static bool valid_password(const char *password)
{
    size_t length = strlen(password);
    if (length == 64) return strspn(password, "0123456789abcdefABCDEF") == length;
    return length == 0 || (length >= 8 && length <= 63);
}

static void load_setup_password(void)
{
    size_t size = sizeof(ap_config.ap.password);
    if (storage_ready &&
        nvs_get_str(storage, "setup_password", (char *)ap_config.ap.password, &size) == ESP_OK &&
        size == 17 && valid_password((const char *)ap_config.ap.password)) {
        return;
    }
    uint32_t random_words[2];
    esp_fill_random(random_words, sizeof(random_words));
    snprintf((char *)ap_config.ap.password, sizeof(ap_config.ap.password),
             "%08" PRIx32 "%08" PRIx32, random_words[0], random_words[1]);
    if (storage_ready &&
        (nvs_set_str(storage, "setup_password", (const char *)ap_config.ap.password) != ESP_OK ||
         nvs_commit(storage) != ESP_OK)) {
        ESP_LOGW(TAG, "Could not persist setup AP password");
    }
}

static esp_err_t change_setup(httpd_req_t *req)
{
    char marker[4];
    char host[24];
    char ip[INET_ADDRSTRLEN];
    struct in_addr address;
    if (!request_address(req, &address) ||
        !inet_ntop(AF_INET, &address, ip, sizeof(ip)) ||
        httpd_req_get_hdr_value_str(req, "Host", host, sizeof(host)) != ESP_OK ||
        strcmp(host, ip) != 0 ||
        httpd_req_get_hdr_value_str(req, "X-E-Nose-Setup", marker, sizeof(marker)) != ESP_OK ||
        strcmp(marker, "1") != 0) {
        return httpd_resp_send_err(req, HTTPD_403_FORBIDDEN, "Open setup using the device IP");
    }
    if (!req->content_len || req->content_len > 640) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request size");
    }
    char body[641];
    size_t received = 0;
    while (received < req->content_len) {
        int length = httpd_req_recv(req, body + received, req->content_len - received);
        if (length <= 0) return ESP_FAIL;
        received += length;
    }
    body[received] = 0;
    cJSON *request = cJSON_ParseWithLengthOpts(body, received + 1, NULL, true);
    const cJSON *action = cJSON_GetObjectItemCaseSensitive(request, "action");
    const cJSON *ssid = cJSON_GetObjectItemCaseSensitive(request, "ssid");
    const cJSON *password = cJSON_GetObjectItemCaseSensitive(request, "password");
    bool local = on_setup_network(req);
    const char *error = NULL;
    const char *code = "400 Bad Request";
    xSemaphoreTake(lock, portMAX_DELAY);
    if (cJSON_IsString(action) && strcmp(action->valuestring, "open") == 0) {
        if (enable_setup() != ESP_OK) {
            code = "503 Service Unavailable";
            error = "Cannot enable setup network";
        }
    } else if (!local) {
        code = "403 Forbidden";
        error = "Join E-Nose-Setup first";
    } else if (cJSON_IsString(action) && strcmp(action->valuestring, "close") == 0) {
        if (state == CONNECTED && saved_to_flash) {
            close_ap_at = esp_timer_get_time() + 2000000;
        } else {
            code = "409 Conflict";
            error = "Connect and save a network first";
        }
    } else if (cJSON_IsString(action) && strcmp(action->valuestring, "connect") == 0 &&
               cJSON_IsString(ssid) && ssid->valuestring[0] && strlen(ssid->valuestring) <= 32 &&
               cJSON_IsString(password) && valid_password(password->valuestring)) {
        if (state == CONNECTING || state == WAIT_DISCONNECT || close_ap_at) {
            code = "409 Conflict";
            error = "Connection in progress";
        } else {
            wifi_credentials_t next = {0};
            strlcpy(next.ssid, ssid->valuestring, sizeof(next.ssid));
            strlcpy(next.password, password->valuestring, sizeof(next.password));
            begin_connection(&next, true);
        }
    } else {
        error = "Invalid Wi-Fi configuration";
    }
    xSemaphoreGive(lock);
    cJSON_Delete(request);
    if (error) return send_error(req, code, error);
    return get_status(req);
}

esp_err_t wifi_setup_register_routes(httpd_handle_t server,
                                    esp_err_t (*file_index)(httpd_req_t *), void *file_context)
{
    show_file_index = file_index;
    const httpd_uri_t routes[] = {
        { .uri = "/", .method = HTTP_GET, .handler = show_index, .user_ctx = file_context },
        { .uri = "/wifi", .method = HTTP_GET, .handler = show_setup },
        { .uri = "/api/wifi/status", .method = HTTP_GET, .handler = get_status },
        { .uri = "/api/wifi/scan", .method = HTTP_GET, .handler = scan_networks },
        { .uri = "/api/wifi", .method = HTTP_POST, .handler = change_setup },
    };
    for (size_t i = 0; i < sizeof(routes) / sizeof(routes[0]); i++) {
        ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server, &routes[i]), TAG, "register Wi-Fi route");
    }
    return ESP_OK;
}

esp_err_t wifi_setup_start(const char *file_base_path)
{
    lock = xSemaphoreCreateMutex();
    if (!lock) return ESP_ERR_NO_MEM;
    ap_netif = esp_netif_create_default_wifi_ap();
    if (!ap_netif || !esp_netif_create_default_wifi_sta()) return ESP_ERR_NO_MEM;
    wifi_init_config_t wifi = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&wifi), TAG, "Wi-Fi initialization");
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "set station mode");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,
                                                  network_event, NULL), TAG, "register Wi-Fi event");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                  network_event, NULL), TAG, "register IP event");

    storage_ready = nvs_open("wifi_setup", NVS_READWRITE, &storage) == ESP_OK;
    wifi_credentials_t saved = {0};
    size_t size = sizeof(saved);
    bool has_saved = storage_ready &&
        nvs_get_blob(storage, "credentials", &saved, &size) == ESP_OK && size == sizeof(saved) &&
        saved.ssid[0] && saved.ssid[32] == 0 && saved.password[64] == 0 &&
        valid_password(saved.password);
    bool migrate_legacy = false;
    if (storage_ready && !has_saved) {
        wifi_config_t previous = {0};
        if (esp_wifi_get_config(WIFI_IF_STA, &previous) == ESP_OK && previous.sta.ssid[0]) {
            memset(&saved, 0, sizeof(saved));
            memcpy(saved.ssid, previous.sta.ssid, sizeof(previous.sta.ssid));
            memcpy(saved.password, previous.sta.password, sizeof(previous.sta.password));
            migrate_legacy = valid_password(saved.password);
        }
    }
    ESP_RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), TAG, "set temporary Wi-Fi storage");

    memcpy(ap_config.ap.ssid, SETUP_SSID, sizeof(SETUP_SSID));
    ap_config.ap.ssid_len = sizeof(SETUP_SSID) - 1;
    ap_config.ap.channel = 1;
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.max_connection = 4;
    ap_config.ap.pmf_cfg.capable = true;
    load_setup_password();

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_APSTA), TAG, "prepare setup AP");
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_AP, &ap_config), TAG, "configure setup AP");
    bool try_saved = has_saved || migrate_legacy;
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(try_saved ? WIFI_MODE_STA : WIFI_MODE_APSTA),
                        TAG, "set initial Wi-Fi mode");
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "start Wi-Fi");

    xSemaphoreTake(lock, portMAX_DELAY);
    if (try_saved) begin_connection(&saved, migrate_legacy);
    esp_err_t err = try_saved ? ESP_OK : enable_setup();
    xSemaphoreGive(lock);
    if (xTaskCreate(manage_network, "wifi_setup", 4096, NULL, 5, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    httpd_handle_t server = NULL;
    ESP_RETURN_ON_ERROR(start_file_server(&server, file_base_path), TAG, "start HTTP server");
    return err;
}
