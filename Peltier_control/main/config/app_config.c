#include "app_config.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "APP_CFG";

static void load_kconfig_defaults(app_config_t *cfg)
{
#if CONFIG_PELTIER_SYSTEM_ENABLE
    cfg->system_enable  = true;
#else
    cfg->system_enable  = false;
#endif
    cfg->temp_on_c      = CONFIG_PELTIER_TEMP_ON_X10 / 10.0f;
    cfg->temp_off_c     = CONFIG_PELTIER_TEMP_OFF_X10 / 10.0f;
    cfg->hum_on_rh      = CONFIG_PELTIER_HUM_ON_X10 / 10.0f;
    cfg->hum_off_rh     = CONFIG_PELTIER_HUM_OFF_X10 / 10.0f;
    cfg->min_on_off_sec = CONFIG_PELTIER_MIN_ON_OFF_SEC;
    cfg->ntc_limit_c    = CONFIG_PELTIER_NTC_LIMIT_X10 / 10.0f;
    cfg->ntc_recover_c  = CONFIG_PELTIER_NTC_RECOVER_X10 / 10.0f;
}

esp_err_t app_config_init(app_config_t *cfg)
{
    if (!cfg) return ESP_ERR_INVALID_ARG;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) return err;

    load_kconfig_defaults(cfg);
    if (cfg->temp_off_c >= cfg->temp_on_c || cfg->hum_off_rh >= cfg->hum_on_rh) {
        ESP_LOGE(TAG, "Chamber temperature and humidity ranges must have lower < upper");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Configuration loaded: T_on=%.1fC, T_off=%.1fC, H_on=%.1f%%, H_off=%.1f%%, MinHold=%lus, T_limit=%.1fC",
             cfg->temp_on_c, cfg->temp_off_c, cfg->hum_on_rh, cfg->hum_off_rh,
             (unsigned long)cfg->min_on_off_sec, cfg->ntc_limit_c);
    return ESP_OK;
}
