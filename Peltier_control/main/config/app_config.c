#include "app_config.h"
#include <math.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "APP_CFG";
static const char *NVS_NAMESPACE = "peltier_cfg";
static const char *LIMITS_KEY = "limits_v1";

static void use_limits(app_config_t *cfg, const peltier_limits_t *limits)
{
    cfg->temp_off_c = limits->temp_low / 10.0f;
    cfg->temp_on_c = limits->temp_high / 10.0f;
    cfg->hum_off_rh = limits->humidity_low / 10.0f;
    cfg->hum_on_rh = limits->humidity_high / 10.0f;
}

peltier_limits_t app_config_get_limits(const app_config_t *cfg)
{
    return (peltier_limits_t) {
        .temp_low = (int16_t)lroundf(cfg->temp_off_c * 10.0f),
        .temp_high = (int16_t)lroundf(cfg->temp_on_c * 10.0f),
        .humidity_low = (int16_t)lroundf(cfg->hum_off_rh * 10.0f),
        .humidity_high = (int16_t)lroundf(cfg->hum_on_rh * 10.0f),
    };
}

peltier_limit_result_t app_config_set_limits(app_config_t *cfg, const peltier_limits_t *limits)
{
    if (!peltier_limits_valid(limits)) return PELTIER_LIMIT_REJECTED;

    nvs_handle_t nvs;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs);
    if (err != ESP_OK) return PELTIER_LIMIT_STORAGE_ERROR;
    err = nvs_set_blob(nvs, LIMITS_KEY, limits, sizeof(*limits));
    if (err == ESP_OK) err = nvs_commit(nvs);
    nvs_close(nvs);
    if (err != ESP_OK) return PELTIER_LIMIT_STORAGE_ERROR;

    use_limits(cfg, limits);
    return PELTIER_LIMIT_APPLIED;
}

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
    peltier_limits_t limits = app_config_get_limits(cfg);
    if (!peltier_limits_valid(&limits)) {
        ESP_LOGE(TAG, "Invalid chamber temperature or humidity ranges in build configuration");
        return ESP_ERR_INVALID_ARG;
    }
    nvs_handle_t nvs;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs) == ESP_OK) {
        size_t size = sizeof(limits);
        if (nvs_get_blob(nvs, LIMITS_KEY, &limits, &size) == ESP_OK &&
            size == sizeof(limits) && peltier_limits_valid(&limits)) {
            use_limits(cfg, &limits);
        }
        nvs_close(nvs);
    }

    ESP_LOGI(TAG, "Configuration loaded: T_on=%.1fC, T_off=%.1fC, H_on=%.1f%%, H_off=%.1f%%, MinHold=%lus, T_limit=%.1fC",
             cfg->temp_on_c, cfg->temp_off_c, cfg->hum_on_rh, cfg->hum_off_rh,
             (unsigned long)cfg->min_on_off_sec, cfg->ntc_limit_c);
    return ESP_OK;
}
