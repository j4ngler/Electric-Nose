#include "PeltierWeb.h"
#include "WifiSetup.h"
#include "peltier_mesh.h"
#include "sdkconfig.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_check.h"

static esp_err_t get_config(httpd_req_t *req)
{
    peltier_config_status_t status = {0};
#if CONFIG_PELTIER_MESH
    peltier_mesh_get_config(&status);
#endif
    char body[200];
    snprintf(body, sizeof(body),
             "{\"online\":%s,\"requestId\":%" PRIu32 ",\"result\":%u,"
             "\"temperatureLow\":%d,\"temperatureHigh\":%d,\"humidityLow\":%d,\"humidityHigh\":%d}",
             status.online ? "true" : "false", status.request_id, (unsigned)status.result,
             status.limits.temp_low, status.limits.temp_high,
             status.limits.humidity_low, status.limits.humidity_high);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_sendstr(req, body);
}

static esp_err_t send_error(httpd_req_t *req, const char *status, const char *message)
{
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, "text/plain; charset=utf-8");
    return httpd_resp_sendstr(req, message);
}

#if CONFIG_PELTIER_MESH
static bool read_limit(const cJSON *json, const char *name, int16_t *limit)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(json, name);
    if (!cJSON_IsNumber(item) || item->valuedouble < INT16_MIN ||
        item->valuedouble > INT16_MAX || item->valuedouble != item->valueint) return false;
    *limit = (int16_t)item->valueint;
    return true;
}
#endif

static esp_err_t set_config(httpd_req_t *req)
{
#if CONFIG_PELTIER_MESH
    char marker[4];
    if (!web_request_has_device_host(req) ||
        httpd_req_get_hdr_value_str(req, "X-E-Nose-Peltier", marker, sizeof(marker)) != ESP_OK ||
        strcmp(marker, "1") != 0) {
        return send_error(req, "403 Forbidden", "Mở trang bằng địa chỉ IP của E-Nose.");
    }
    char body[192];
    if (!req->content_len || req->content_len >= sizeof(body)) {
        return send_error(req, "400 Bad Request", "Dữ liệu ngưỡng không hợp lệ.");
    }
    size_t received = 0;
    while (received < req->content_len) {
        int length = httpd_req_recv(req, body + received, req->content_len - received);
        if (length <= 0) return ESP_FAIL;
        received += length;
    }
    body[received] = 0;
    cJSON *json = cJSON_ParseWithLengthOpts(body, received + 1, NULL, true);
    peltier_limits_t limits;
    bool valid = read_limit(json, "temperatureLow", &limits.temp_low) &&
                 read_limit(json, "temperatureHigh", &limits.temp_high) &&
                 read_limit(json, "humidityLow", &limits.humidity_low) &&
                 read_limit(json, "humidityHigh", &limits.humidity_high) &&
                 peltier_limits_valid(&limits);
    cJSON_Delete(json);
    if (!valid) return send_error(req, "400 Bad Request", "Ngưỡng thấp phải nhỏ hơn ngưỡng cao.");

    uint32_t request_id;
    esp_err_t err = peltier_mesh_set_config(&limits, &request_id);
    if (err == ESP_ERR_NOT_FINISHED) {
        return send_error(req, "409 Conflict", "Một yêu cầu đang chờ bo Peltier xác nhận.");
    }
    if (err != ESP_OK) {
        return send_error(req, "503 Service Unavailable", "Bo Peltier chưa kết nối.");
    }
    snprintf(body, sizeof(body), "{\"requestId\":%" PRIu32 "}", request_id);
    httpd_resp_set_status(req, "202 Accepted");
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_sendstr(req, body);
#else
    return send_error(req, "503 Service Unavailable", "Bo E-Nose chưa chạy firmware Mesh.");
#endif
}

esp_err_t peltier_web_register_routes(httpd_handle_t server)
{
    const httpd_uri_t routes[] = {
        { .uri = "/api/peltier/limits", .method = HTTP_GET, .handler = get_config },
        { .uri = "/api/peltier/limits", .method = HTTP_POST, .handler = set_config },
    };
    for (size_t i = 0; i < sizeof(routes) / sizeof(routes[0]); i++) {
        ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server, &routes[i]), "PELTIER_WEB",
                            "register Peltier route");
    }
    return ESP_OK;
}
