#include "telemetry_logger.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"

#if CONFIG_PELTIER_ENABLE_CAN
#include "driver/twai.h"
static int64_t s_last_can_rx_ms = 0;
#endif

static const char *TAG = "TELEMETRY";

esp_err_t telemetry_logger_init(void)
{
#if CONFIG_PELTIER_ENABLE_CAN
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        CONFIG_PELTIER_CAN_TX_GPIO, CONFIG_PELTIER_CAN_RX_GPIO, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err == ESP_OK) {
        err = twai_start();
    }
    s_last_can_rx_ms = esp_timer_get_time() / 1000;
    ESP_LOGI(TAG, "Initialize CAN (TWAI) telemetry: %s", esp_err_to_name(err));
    return err;
#else
    return ESP_OK;
#endif
}

void telemetry_log_uart(const peltier_fsm_state_t *fsm)
{
    if (!fsm) return;
    ESP_LOGI(TAG, "[STATUS] T: %.2f C | H: %.2f %%RH | T_hot(GPIO34): %.2f C | Peltier(GPIO%d): %s | ErrFlags: 0x%02lX",
             fsm->filtered_temp_c,
             fsm->filtered_hum_rh,
             fsm->temp_hot_c,
             CONFIG_PELTIER_AOD4184_GPIO,
             fsm->peltier_on ? "ON" : "OFF",
             (unsigned long)fsm->error_flags);
}

void telemetry_send_can(const peltier_fsm_state_t *fsm)
{
#if CONFIG_PELTIER_ENABLE_CAN
    if (!fsm) return;
    int16_t t_x10 = (int16_t)(fsm->filtered_temp_c * 10.0f);
    int16_t h_x10 = (int16_t)(fsm->filtered_hum_rh * 10.0f);
    int16_t thot_x10 = (int16_t)(fsm->temp_hot_c * 10.0f);

    twai_message_t tx_msg = {
        .identifier = 0x310,
        .data_length_code = 8,
        .data = {
            (uint8_t)(t_x10 >> 8), (uint8_t)(t_x10 & 0xFF),
            (uint8_t)(h_x10 >> 8), (uint8_t)(h_x10 & 0xFF),
            (uint8_t)(thot_x10 >> 8), (uint8_t)(thot_x10 & 0xFF),
            fsm->peltier_on ? 1 : 0,
            (uint8_t)(fsm->error_flags & 0xFF)
        }
    };
    twai_transmit(&tx_msg, pdMS_TO_TICKS(20));
#else
    (void)fsm;
#endif
}

bool telemetry_check_can_alive(void)
{
#if CONFIG_PELTIER_ENABLE_CAN
    twai_message_t rx_msg;
    while (twai_receive(&rx_msg, 0) == ESP_OK) {
        s_last_can_rx_ms = esp_timer_get_time() / 1000;
    }
    int64_t elapsed = (esp_timer_get_time() / 1000) - s_last_can_rx_ms;
    return (elapsed <= CONFIG_PELTIER_CAN_TIMEOUT_MS);
#else
    return true;
#endif
}