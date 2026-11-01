#include "peltier_aod4184.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#if CONFIG_PELTIER_AOD4184_ACTIVE_HIGH
#define AOD4184_LEVEL_ON    1
#define AOD4184_LEVEL_OFF   0
#else
#define AOD4184_LEVEL_ON    0
#define AOD4184_LEVEL_OFF   1
#endif

static const char *TAG = "AOD4184_DRV";
static bool s_is_on = false;

esp_err_t peltier_aod4184_init(void)
{
    gpio_reset_pin(CONFIG_PELTIER_AOD4184_GPIO);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << CONFIG_PELTIER_AOD4184_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) return err;

    gpio_set_level(CONFIG_PELTIER_AOD4184_GPIO, AOD4184_LEVEL_OFF);
    s_is_on = false;

    ESP_LOGI(TAG, "Initialized control for Module AOD4184 (Opto PC817 - PWM labeled) at GPIO%d -> Default: OFF.",
             CONFIG_PELTIER_AOD4184_GPIO);
    return ESP_OK;
}

void peltier_aod4184_set_state(bool enable)
{
    gpio_set_level(CONFIG_PELTIER_AOD4184_GPIO, enable ? AOD4184_LEVEL_ON : AOD4184_LEVEL_OFF);
    s_is_on = enable;
}

bool peltier_aod4184_get_state(void)
{
    return s_is_on;
}