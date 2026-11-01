#include "app_config.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "APP_CFG";
static const char *NVS_NAMESPACE = "peltier_cfg";

static void load_kconfig_defaults(app_config_t *cfg)
{
    cfg->system_enable  = CONFIG_PELTIER_SYSTEM_ENABLE;
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
    ESP_ERROR_CHECK(err);

    load_kconfig_defaults(cfg);

    nvs_handle_t handle;
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "No NVS data found, saving default configuration from Kconfig.");
        return app_config_save(cfg);
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
        return err;
    }

    size_t required_size = sizeof(app_config_t);
    err = nvs_get_blob(handle, "cfg_blob", cfg, &required_size);
    nvs_close(handle);

    if (err != ESP_OK || required_size != sizeof(app_config_t)) {
        ESP_LOGW(TAG, "Invalid NVS data, restoring default Kconfig.");
        load_kconfig_defaults(cfg);
        return app_config_save(cfg);
    }

    ESP_LOGI(TAG, "Configuration loaded: T_on=%.1fC, T_off=%.1fC, H_on=%.1f%%, H_off=%.1f%%, MinHold=%lus, T_limit=%.1fC",
             cfg->temp_on_c, cfg->temp_off_c, cfg->hum_on_rh, cfg->hum_off_rh,
             (unsigned long)cfg->min_on_off_sec, cfg->ntc_limit_c);
    return ESP_OK;
}

esp_err_t app_config_save(const app_config_t *cfg)
{
    if (!cfg) return ESP_ERR_INVALID_ARG;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(handle, "cfg_blob", cfg, sizeof(app_config_t));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}