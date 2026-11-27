#include "peltier_mesh.h"
#include "sdkconfig.h"

#if CONFIG_PELTIER_MESH
#include <inttypes.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_mesh.h"
#include "esp_netif.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define MESH_LEASE_US (5 * 1000 * 1000)
#define CONFIG_TIMEOUT_US (10 * 1000 * 1000)
#define MESH_REFRESH_US (1000 * 1000)

enum { WIRE_VERSION = 2, WIRE_COMMAND = 1, WIRE_STATUS = 2 };

typedef struct {
    uint8_t version;
    uint8_t kind;
    uint8_t enabled;
    uint8_t output_on;
    uint32_t session;
    uint32_t sequence;
    uint32_t errors;
    float temp_c;
    float humidity;
    float hot_c;
    uint32_t config_id;
    uint32_t config_result;
    peltier_limits_t limits;
} mesh_packet_t;

_Static_assert(sizeof(mesh_packet_t) == 44, "Unexpected ESP32 mesh packet layout");

static const char *TAG = "PELTIER_MESH";
static const uint8_t mesh_id[6] = {0x02, 0x45, 0x4e, 0x4f, 0x53, 0x45};
static mesh_addr_t peer;
static esp_netif_t *root_netif;
static atomic_bool connected;
static atomic_bool node_reset_pending;
static bool started;
static uint32_t node_session;
static uint32_t node_sequence;
static bool node_enabled;
static int64_t last_command_us;
static uint32_t node_config_id;
static peltier_limit_result_t node_config_result;
static SemaphoreHandle_t config_lock;
static peltier_config_status_t config_status;
static peltier_limits_t requested_limits;
static int64_t config_deadline_us;

void peltier_mesh_get_config(peltier_config_status_t *status)
{
    *status = (peltier_config_status_t){0};
    if (!config_lock) return;
    xSemaphoreTake(config_lock, portMAX_DELAY);
    *status = config_status;
    xSemaphoreGive(config_lock);
}

esp_err_t peltier_mesh_set_config(const peltier_limits_t *limits, uint32_t *request_id)
{
    if (!peltier_limits_valid(limits)) return ESP_ERR_INVALID_ARG;
    if (!config_lock) return ESP_ERR_INVALID_STATE;
    esp_err_t err = ESP_ERR_INVALID_STATE;
    xSemaphoreTake(config_lock, portMAX_DELAY);
    if (config_status.online && atomic_load(&connected)) {
        err = ESP_ERR_NOT_FINISHED;
        if (config_status.result != PELTIER_LIMIT_PENDING) {
            if (++config_status.request_id == 0) ++config_status.request_id;
            *request_id = config_status.request_id;
            config_status.result = PELTIER_LIMIT_PENDING;
            requested_limits = *limits;
            config_deadline_us = esp_timer_get_time() + CONFIG_TIMEOUT_US;
            err = ESP_OK;
        }
    }
    xSemaphoreGive(config_lock);
    return err;
}

static void root_got_ip(void *arg, esp_event_base_t base, int32_t event, void *data)
{
    const ip_event_got_ip_t *got_ip = data;
    if (got_ip->esp_netif == root_netif) {
        ESP_LOGI(TAG, "E-Nose LAN IP: " IPSTR, IP2STR(&got_ip->ip_info.ip));
    }
}

static void mesh_event(void *arg, esp_event_base_t base, int32_t event, void *data)
{
    if (event == MESH_EVENT_PARENT_CONNECTED) {
        const bool is_root = esp_mesh_is_root();
        if (is_root != (root_netif != NULL)) {
            ESP_LOGE(TAG, "Unexpected mesh role");
            return;
        }
        if (is_root) {
            esp_netif_dhcpc_stop(root_netif);
            esp_netif_dhcpc_start(root_netif);
        }
        atomic_store(&connected, true);
        ESP_LOGI(TAG, "Mesh parent connected as %s", is_root ? "root" : "node");
    } else if (event == MESH_EVENT_PARENT_DISCONNECTED || event == MESH_EVENT_STOPPED) {
        atomic_store(&node_reset_pending, true);
        atomic_store(&connected, false);
    }
}

static esp_err_t send_packet(mesh_packet_t *packet)
{
    mesh_data_t data = {
        .data = (uint8_t *)packet,
        .size = sizeof(*packet),
        .proto = MESH_PROTO_BIN,
        .tos = MESH_TOS_P2P,
    };
    return esp_mesh_send(&peer, &data, MESH_DATA_P2P | MESH_DATA_NONBLOCK, NULL, 0);
}

static bool receive_packet(mesh_packet_t *packet, uint8_t kind, int wait_ms)
{
    uint8_t buffer[MESH_MTU];
    mesh_addr_t sender;
    mesh_data_t data = { .data = buffer, .size = sizeof(buffer) };
    int flags;
    if (esp_mesh_recv(&sender, &data, wait_ms, &flags, NULL, 0) != ESP_OK ||
        data.size != sizeof(*packet) || memcmp(peer.addr, sender.addr, sizeof(peer.addr))) {
        return false;
    }
    memcpy(packet, buffer, sizeof(*packet));
    return packet->version == WIRE_VERSION && packet->kind == kind &&
           packet->enabled <= 1 && packet->output_on <= 1;
}

static void root_task(void *arg)
{
    mesh_packet_t command = { .version = WIRE_VERSION, .kind = WIRE_COMMAND, .sequence = 1 };
    do {
        command.session = esp_random();
    } while (!command.session);

    bool ready = false;
    int64_t last_status_us = 0;
    int64_t next_send_us = 0;
    ESP_LOGI(TAG, "Root UART: 1 = enable, 0 = disable");
    for (;;) {
        if (!atomic_load(&connected) || esp_timer_get_time() - last_status_us >= MESH_LEASE_US) {
            if (ready) ESP_LOGW(TAG, "Peltier offline; enable cleared");
            if (command.enabled) command.sequence++;
            command.enabled = 0;
            ready = false;
        }

        int key = getchar();
        if (key == EOF) clearerr(stdin);
        if (key == '0' || (key == '1' && ready)) {
            command.enabled = (key == '1');
            command.sequence++;
            next_send_us = 0;
            ready = false;  // Require the new command to be acknowledged.
        } else if (key == '1') {
            ESP_LOGW(TAG, "Wait for Peltier ACK before enabling");
        }
        int64_t now = esp_timer_get_time();
        xSemaphoreTake(config_lock, portMAX_DELAY);
        if (!atomic_load(&connected) || now - last_status_us >= MESH_LEASE_US) {
            config_status.online = false;
        }
        if (config_status.result == PELTIER_LIMIT_PENDING &&
            (!config_status.online || now >= config_deadline_us)) {
            config_status.result = PELTIER_LIMIT_TIMEOUT;
        }
        command.config_id = config_status.result == PELTIER_LIMIT_PENDING ? config_status.request_id : 0;
        command.limits = requested_limits;
        xSemaphoreGive(config_lock);
        if (atomic_load(&connected) && now >= next_send_us) {
            send_packet(&command);
            next_send_us = now + MESH_REFRESH_US;
        }

        mesh_packet_t status;
        if (atomic_load(&connected) && receive_packet(&status, WIRE_STATUS, 100) &&
            status.session == command.session && peltier_limits_valid(&status.limits)) {
            last_status_us = esp_timer_get_time();
            ready = status.session == command.session && status.sequence == command.sequence &&
                    status.enabled == command.enabled;
            xSemaphoreTake(config_lock, portMAX_DELAY);
            config_status.online = true;
            config_status.limits = status.limits;
            if (config_status.result == PELTIER_LIMIT_PENDING &&
                status.config_id == config_status.request_id &&
                status.config_result >= PELTIER_LIMIT_APPLIED &&
                status.config_result <= PELTIER_LIMIT_STORAGE_ERROR) {
                config_status.result = status.config_result;
                if (status.config_result == PELTIER_LIMIT_APPLIED &&
                    memcmp(&requested_limits, &status.limits, sizeof(status.limits)) != 0) {
                    config_status.result = PELTIER_LIMIT_REJECTED;
                }
            }
            xSemaphoreGive(config_lock);
            ESP_LOGI(TAG, "seq=%" PRIu32 " ack=%d enable=%d out=%d T=%.2f RH=%.2f hot=%.2f errors=0x%08" PRIx32,
                     status.sequence, ready, status.enabled, status.output_on,
                     status.temp_c, status.humidity, status.hot_c, status.errors);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

bool peltier_mesh_node_enabled(peltier_limit_result_t (*apply)(const peltier_limits_t *))
{
    if (!started) return false;
    int64_t now = esp_timer_get_time();
    if (atomic_exchange(&node_reset_pending, false) || now - last_command_us >= MESH_LEASE_US) {
        node_enabled = false;
    }
    if (!atomic_load(&connected)) return false;

    mesh_packet_t command;
    for (int i = 0; i < 8 && receive_packet(&command, WIRE_COMMAND, 0); i++) {
        if (!command.session || !command.sequence) continue;
        if (command.session != node_session) {
            node_enabled = false;
            if (command.enabled) continue; // Establish a new session with OFF first.
            node_session = command.session;
            node_sequence = 0;
            node_config_id = 0;
            node_config_result = PELTIER_LIMIT_IDLE;
        }
        if (command.sequence < node_sequence) continue;
        if (command.sequence > node_sequence) {
            node_sequence = command.sequence;
            node_enabled = command.enabled;
        }
        if (!command.enabled) node_enabled = false;
        last_command_us = now;
        if (command.config_id > node_config_id) {
            node_config_id = command.config_id;
            node_config_result = peltier_limits_valid(&command.limits) && apply
                ? apply(&command.limits) : PELTIER_LIMIT_REJECTED;
        }
    }
    return node_enabled;
}

void peltier_mesh_send_status(const peltier_mesh_status_t *status)
{
    if (!started || !atomic_load(&connected)) return;
    mesh_packet_t packet = {
        .version = WIRE_VERSION,
        .kind = WIRE_STATUS,
        .enabled = status->enabled,
        .output_on = status->output_on,
        .session = node_session,
        .sequence = node_sequence,
        .errors = status->errors,
        .temp_c = status->temp_c,
        .humidity = status->humidity,
        .hot_c = status->hot_c,
        .config_id = node_config_id,
        .config_result = node_config_result,
        .limits = status->limits,
    };
    send_packet(&packet);
}

#define RETURN_ON_ERROR(call) do { esp_err_t err = (call); if (err != ESP_OK) return err; } while (0)

esp_err_t peltier_mesh_start(bool root)
{
    uint8_t mac[6];
    RETURN_ON_ERROR(esp_read_mac(mac, ESP_MAC_WIFI_STA));
    ESP_LOGI(TAG, "%s STA MAC %02x:%02x:%02x:%02x:%02x:%02x", root ? "Root" : "Node",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    const size_t ssid_len = strlen(CONFIG_PELTIER_MESH_ROUTER);
    const size_t router_password_len = strlen(CONFIG_PELTIER_MESH_ROUTER_PASSWORD);
    const size_t mesh_password_len = strlen(CONFIG_PELTIER_MESH_PASSWORD);
    unsigned int bytes[6];
    char extra;
    if (!ssid_len || ssid_len > sizeof(cfg.router.ssid) ||
        router_password_len >= sizeof(cfg.router.password) ||
        mesh_password_len < 8 || mesh_password_len >= sizeof(cfg.mesh_ap.password) ||
        sscanf(CONFIG_PELTIER_MESH_PEER, "%2x:%2x:%2x:%2x:%2x:%2x%c",
               &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5], &extra) != 6) {
        ESP_LOGE(TAG, "Configure router, mesh password and peer STA MAC in menuconfig");
        return ESP_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < sizeof(peer.addr); i++) peer.addr[i] = bytes[i];

    if (root) {
        config_lock = xSemaphoreCreateMutex();
        if (!config_lock) return ESP_ERR_NO_MEM;
        config_status.request_id = esp_random() & UINT32_C(0x7fffffff);
        RETURN_ON_ERROR(esp_netif_create_default_wifi_mesh_netifs(&root_netif, NULL));
        RETURN_ON_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, root_got_ip, NULL));
    }
    wifi_init_config_t wifi = WIFI_INIT_CONFIG_DEFAULT();
    RETURN_ON_ERROR(esp_wifi_init(&wifi));
    RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    RETURN_ON_ERROR(esp_wifi_start());
    RETURN_ON_ERROR(esp_mesh_init());
    RETURN_ON_ERROR(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, mesh_event, NULL));
    RETURN_ON_ERROR(esp_mesh_set_max_layer(2));
    RETURN_ON_ERROR(esp_mesh_set_ap_authmode(WIFI_AUTH_WPA2_PSK));
    cfg.channel = CONFIG_PELTIER_MESH_CHANNEL;
    memcpy(cfg.mesh_id.addr, mesh_id, sizeof(mesh_id));
    cfg.router.ssid_len = ssid_len;
    memcpy(cfg.router.ssid, CONFIG_PELTIER_MESH_ROUTER, ssid_len);
    memcpy(cfg.router.password, CONFIG_PELTIER_MESH_ROUTER_PASSWORD, router_password_len);
    cfg.mesh_ap.max_connection = 1;
    memcpy(cfg.mesh_ap.password, CONFIG_PELTIER_MESH_PASSWORD, mesh_password_len);
    RETURN_ON_ERROR(esp_mesh_set_config(&cfg));
    RETURN_ON_ERROR(esp_mesh_fix_root(true));
    if (root) {
        wifi_config_t parent = { .sta.channel = cfg.channel };
        memcpy(parent.sta.ssid, cfg.router.ssid, ssid_len);
        memcpy(parent.sta.password, cfg.router.password, router_password_len);
        RETURN_ON_ERROR(esp_mesh_set_parent(&parent, NULL, MESH_ROOT, MESH_ROOT_LAYER));
    }
    RETURN_ON_ERROR(esp_mesh_start());
    if (root && xTaskCreate(root_task, "peltier_mesh", 6144, NULL, 5, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    started = true;
    return ESP_OK;
}
#endif
