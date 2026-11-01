#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    bool system_enable;         // Flag to enable the system
    float temp_on_c;            // Temperature threshold to turn on (default 25.0 C)
    float temp_off_c;           // Temperature threshold to turn off (default 20.0 C)
    float hum_on_rh;            // Humidity threshold to turn on (default 65.0 %)
    float hum_off_rh;           // Humidity threshold to turn off (default 60.0 %)
    uint32_t min_on_off_sec;    // Minimum state hold time (10 - 30s)
    float ntc_limit_c;          // Temperature threshold for NTC overheat protection (default 80.0 C)
    float ntc_recover_c;        // Temperature threshold for NTC safe recovery (default 65.0 C)
} app_config_t;

esp_err_t app_config_init(app_config_t *cfg);
esp_err_t app_config_save(const app_config_t *cfg);

#endif // APP_CONFIG_H