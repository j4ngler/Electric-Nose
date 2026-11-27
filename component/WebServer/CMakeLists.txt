set(app_src FileServer.c WifiSetup.c)
set(pre_req vfs fatfs esp_http_server)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req}
                    PRIV_REQUIRES esp_wifi esp_event esp_netif esp_timer esp_hw_support nvs_flash json lwip
                    EMBED_FILES "favicon.ico" "upload_script.html" "wifi_setup.html")
