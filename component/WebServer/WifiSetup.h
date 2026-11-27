#pragma once

#include "esp_http_server.h"

esp_err_t wifi_setup_start(const char *file_base_path);
esp_err_t wifi_setup_register_routes(httpd_handle_t server,
                                    esp_err_t (*file_index)(httpd_req_t *), void *file_context);
