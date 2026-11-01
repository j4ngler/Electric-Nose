#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "app_config.h"
#include "sht35_ao3401.h"
#include "ntc_adc.h"
#include "peltier_aod4184.h"
#include "moving_avg_filter.h"
#include "peltier_fsm.h"
#include "telemetry_logger.h"

static const char *TAG = "MAIN";
static TaskHandle_t s_control_task_handle = NULL;
static app_config_t s_app_cfg;

#if CONFIG_PELTIER_SELF_TEST_ENABLE
static bool run_hardware_self_test(const app_config_t *cfg)
{
    bool all_passed = true;

    // 1: Check the initial safety lock status of AOD4184 (GPIO13) & AO3401 (GPIO4)
    peltier_aod4184_set_state(false);
    sht35_ao3401_power_off_bus_float();
    ESP_LOGI(TAG, "[POST 1/3] Locked AOD4184 (GPIO%d=LOW) & floated SHT35 bus (AO3401 GPIO%d=HIGH).",
             CONFIG_PELTIER_AOD4184_GPIO, CONFIG_PELTIER_AO3401_GPIO);

    // Step 2: Check the NTC B3950 10K sensor on D34 pin (GPIO34)
    float test_thot = 0.0f;
    esp_err_t ntc_err = ntc_adc_read_temp(&test_thot);
    bool ntc_safe = (ntc_err == ESP_OK) && (test_thot < cfg->ntc_limit_c);
    if (ntc_err == ESP_OK) {
        ESP_LOGI(TAG, "[POST 2/3] NTC (GPIO34): PASS -> T_hot = %.2f C (Cutoff threshold: %.1f C)",
                 test_thot, cfg->ntc_limit_c);
        if (!ntc_safe) {
            ESP_LOGE(TAG, "[POST 2/3] WARNING: Hot-side temperature is above the safe threshold!");
            all_passed = false;
        }
    } else {
        ESP_LOGE(TAG, "[POST 2/3] NTC (GPIO34): FAIL -> Open-circuit or shorted NTC sensor fault!");
        all_passed = false;
    }

    // Step 3: Check the AO3401 power switch (GPIO4) & read the I2C SHT35 sensor (SDA=21, SCL=22, RST=23)
    float test_t = 0.0f, test_h = 0.0f;
    esp_err_t sht_err = sht35_ao3401_read_temp_hum(&test_t, &test_h);
    if (sht_err == ESP_OK) {
        ESP_LOGI(TAG, "[POST 3/3] AO3401 + SHT35: PASS -> T = %.2f C, H = %.2f %%RH (CRC-8 OK)",
                 test_t, test_h);
    } else {
        ESP_LOGE(TAG, "[POST 3/3] AO3401 + SHT35: FAIL -> Error code: %s (Check J1 AO3401 or I2C wiring)",
                 esp_err_to_name(sht_err));
        all_passed = false;
    }

    // Step 4: Pulse 2 test signals on Opto PC817 & LED L3 (GPIO13 - PWM label) if NTC is safe
    if (ntc_safe) {
        ESP_LOGI(TAG, "[POST ACTUATOR] Pulse test 2 bursts (%d ms) on GPIO%d (PWM) -> Observe LED L3...",
                 CONFIG_PELTIER_SELF_TEST_PULSE_MS, CONFIG_PELTIER_AOD4184_GPIO);
        for (int i = 0; i < 2; i++) {
            peltier_aod4184_set_state(true);
            vTaskDelay(pdMS_TO_TICKS(CONFIG_PELTIER_SELF_TEST_PULSE_MS));
            peltier_aod4184_set_state(false);
            vTaskDelay(pdMS_TO_TICKS(CONFIG_PELTIER_SELF_TEST_PULSE_MS));
        }
        ESP_LOGI(TAG, "[POST ACTUATOR] Returned GPIO%d (PWM) pin to the safe OFF state.",
                 CONFIG_PELTIER_AOD4184_GPIO);
    } else {
        ESP_LOGW(TAG, "[POST ACTUATOR] Skipped AOD4184 test (GPIO%d) because the NTC reported a fault or overheat.",
                 CONFIG_PELTIER_AOD4184_GPIO);
    }
    ESP_LOGI(TAG, "   SELF-TEST RESULT: %s", all_passed ? "COMPLETE (ALL PASS)" : "HARDWARE ERROR (CHECK LOG)");
    return all_passed;
}
#endif

static bool IRAM_ATTR sample_timer_isr_cb(gptimer_handle_t timer,
                                          const gptimer_alarm_event_data_t *edata,
                                          void *user_ctx)
{
    BaseType_t high_task_awoken = pdFALSE;
    if (s_control_task_handle != NULL) {
        vTaskNotifyGiveFromISR(s_control_task_handle, &high_task_awoken);
    }
    return (high_task_awoken == pdTRUE);
}

static void init_hardware_sample_timer(uint32_t period_ms)
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 MHz (1 tick = 1 us)
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = sample_timer_isr_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = (uint64_t)period_ms * 1000ULL,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    ESP_LOGI(TAG, "Hardware Timer ISR started with sample period %lu ms.", (unsigned long)period_ms);
}

static void peltier_control_task(void *pvParameters)
{
    moving_avg_filter_t temp_filter;
    moving_avg_filter_t hum_filter;
    peltier_fsm_state_t fsm_state;

    moving_avg_init(&temp_filter, CONFIG_PELTIER_FILTER_WINDOW);
    moving_avg_init(&hum_filter, CONFIG_PELTIER_FILTER_WINDOW);
    peltier_fsm_init(&fsm_state);

    while (1) {
        // Wait for the wake signal from the gptimer ISR interrupt
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        float raw_temp = 0.0f, raw_hum = 0.0f, temp_hot = 0.0f;
        float filt_temp = fsm_state.filtered_temp_c;
        float filt_hum  = fsm_state.filtered_hum_rh;

        // 1. Read SHT35: enable AO3401 (GPIO4), drive RST (GPIO23) HIGH, read I2C (GPIO21/22), then float all lines
        bool sht35_ok = (sht35_ao3401_read_temp_hum(&raw_temp, &raw_hum) == ESP_OK);
        if (sht35_ok) {
            filt_temp = moving_avg_update(&temp_filter, raw_temp);
            filt_hum  = moving_avg_update(&hum_filter, raw_hum);
        }

        // 2. Read the hot-side NTC B3950 10K temperature sensor on D34 pin (GPIO34)
        bool ntc_ok = (ntc_adc_read_temp(&temp_hot) == ESP_OK);

        // 3. Check CAN communication status (if enabled)
        bool can_ok = telemetry_check_can_alive();

        // 4. Update the FSM state machine & drive the signal on D13 (GPIO13 - PWM label)
        peltier_fsm_update(&fsm_state, &s_app_cfg, filt_temp, filt_hum, temp_hot, sht35_ok, ntc_ok, can_ok);

        // 5. Output UART monitoring logs & transmit CAN telemetry packets
        telemetry_log_uart(&fsm_state);
        telemetry_send_can(&fsm_state);
    }
}

void app_main(void)
{
    // 1. Initialize the AOD4184 control pin (GPIO13 - PWM) to the safe OFF state immediately on power-up
    ESP_ERROR_CHECK(peltier_aod4184_init());

    // 2. Initialize NVS / Kconfig settings
    ESP_ERROR_CHECK(app_config_init(&s_app_cfg));

    // 3. Initialize AO3401 (GPIO4), SHT35, NTC (GPIO34) & telemetry
    ESP_ERROR_CHECK(sht35_ao3401_init());
    ESP_ERROR_CHECK(ntc_adc_init());
    ESP_ERROR_CHECK(telemetry_logger_init());

#if CONFIG_PELTIER_SELF_TEST_ENABLE
    // 4. Execute the hardware self-test (POST) before entering the main control loop
    run_hardware_self_test(&s_app_cfg);
#endif

    // 5. Create the FreeRTOS task for sensor reading and Peltier control
    xTaskCreatePinnedToCore(
        peltier_control_task,
        "peltier_ctrl_task",
        4096,
        NULL,
        5,
        &s_control_task_handle,
        1
    );

    // 6. Start the periodic Hardware Timer ISR
    init_hardware_sample_timer(CONFIG_PELTIER_SAMPLE_PERIOD_MS);
}