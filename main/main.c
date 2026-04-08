
/*------------------------------------ INCLUDE LIBRARY ------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <sys/param.h>
#include <sys/time.h>

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_cpu.h"
#include "esp_mem.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_chip_info.h"
#include "esp_pm.h"

#include "esp_flash.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_attr.h"
#include <spi_flash_mmap.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include "esp_eap_client.h"
#include "esp_smartconfig.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "driver/uart.h"
#include "driver/i2c.h"
#include "driver/spi_common.h"
#include "driver/gptimer.h"
#include "esp_spiffs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"

#include "sdcard.h"
#include "DS3231Time.h"
#include "datamanager.h"
#include "DeviceManager.h"
#include "sntp_sync.h"
#include "ADS111x.h"
#include "sht3x.h"
#include "FileServer.h"
#include "esp_lcd_ili9341.h"
#include "extended_ili9341.h"
#include "ui/ui.h"
#include "main_common.h"

/*------------------------------------ DEFINE ------------------------------------ */

__attribute__((unused)) static const char *TAG = "Main";

#define PERIOD_GET_DATA_FROM_SENSOR (TickType_t)(500 / portTICK_PERIOD_MS)
#define PERIOD_SAVE_DATA_SENSOR_TO_SDCARD (TickType_t)(50 / portTICK_PERIOD_MS)
#define SAMPLING_TIME (TickType_t)(2400000 / portTICK_PERIOD_MS) // mili seconds
#define CLEAN_CHAMBER_TIME (10)                                  // seconds

#define NO_WAIT (TickType_t)(0)
#define WAIT_10_TICK (TickType_t)(10 / portTICK_PERIOD_MS)
#define WAIT_100_TICK (TickType_t)(100 / portTICK_PERIOD_MS)

#define QUEUE_SIZE 10U
#define DATA_SENSOR_MIDLEWARE_QUEUE_SIZE 20

TaskHandle_t getDataFromSensorTask_handle = NULL;
TaskHandle_t saveDataSensorToSDcardTask_handle = NULL;
TaskHandle_t sntp_syncTimeTask_handle = NULL;
TaskHandle_t allocateDataForMultipleQueuesTask_handle = NULL;
TaskHandle_t smartConfigTask_handle = NULL;

SemaphoreHandle_t getDataSensor_semaphore = NULL;
SemaphoreHandle_t SDcard_semaphore = NULL;
SemaphoreHandle_t writeDataToSDcardNoWifi_semaphore = NULL;
SemaphoreHandle_t stop_clean_chamber_semaphore = NULL;           // Semaphore for stop clean chamber stage in measurement process
SemaphoreHandle_t monitor_temperature_humidity_semaphore = NULL; // Semaphore for monitor temperature and humidity task

QueueHandle_t dataSensorSentToSD_queue = NULL;
// Event group
EventGroupHandle_t wifi_control_eventGroup;
EventGroupHandle_t measure_control_eventGroup;

static char nameFileSaveData[21] = {0};
static const char base_path[] = MOUNT_POINT;
httpd_handle_t file_server = NULL; // handle for file server

gptimer_handle_t clean_chamber_Timer = NULL; // Timer for cleaning sensor chamber stage

/*------------------------------------ Define devices ------------------------------------ */
static i2c_dev_t ds3231_device = {0};
static i2c_dev_t ads111x_devices[CONFIG_ADS111X_DEVICE_COUNT] = {0};
static sht3x_t sht30_sensor = {0};
i2c_dev_t pcf8575_device = {0};
moduleError_st moduleError;

// I2C addresses for ADS1115
const uint8_t addresses[CONFIG_ADS111X_DEVICE_COUNT] = {
    ADS111X_ADDR_SDA,
    ADS111X_ADDR_GND};

/*------------------------------------ WIFI ------------------------------------ */
// Define Wi-Fi
#define WIFI_connect_max_attemp_number CONFIG_MAX_RECONNECT_ATTEMP_NUMBER
static const char *SC_TAG = "SMART CONFIG";
static smartconfig_start_config_t smart_cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
static uint8_t attemp_count = 0;
bool change_AP = false;
bool is_connected = false;
static uint8_t attemp_count_1 = 0;
static wifi_config_t wifi_config;
uint8_t old_ssid[32] = CONFIG_DEFAULT_AP_SSID;
uint8_t old_password[64] = CONFIG_DEFAULT_AP_PASSWORD;
uint8_t old_bssid[6] = {0x34, 0xE8, 0x94, 0xF6, 0xAA, 0xD7};

/**
 * @brief Manage wifi state base on event from wifi_control_eventGroup
 * WIFI_OFF_BIT: turn off wifi and smartconfig
 * WIFI_ON_BIT: turn on wifi
 */
static void wifi_control_task(void *parm)
{
    wifi_control_eventGroup = xEventGroupCreate();
    for (;;)
    {
        EventBits_t uxBits = xEventGroupWaitBits(
            wifi_control_eventGroup,
            WIFI_OFF_BIT | WIFI_ON_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if (uxBits & WIFI_OFF_BIT)
        {
            // esp_wifi_disconnect();
            esp_smartconfig_stop();
            esp_wifi_stop();
            esp_wifi_deinit();
        }
        else
        {
            if (uxBits & WIFI_ON_BIT)
            {
                wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
                esp_wifi_init(&cfg);
                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_STA));
                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start());
            }
        }
    }
}

static void Wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
        {
            ui_wifi_setting_label_state_change(WIFI_START, "Start connecting");
            ESP_ERROR_CHECK(esp_smartconfig_start(&smart_cfg));
            break;
        }
        case WIFI_EVENT_STA_STOP:
        {
            ui_wifi_setting_label_state_change(WIFI_NOT_CONNECTED, "Not connected");

            stop_file_server(file_server);
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            if (is_connected == true)
            {
                is_connected = false;
                ui_update_device_icon_state(wifi_icon, false);
                char *ssid_str = (char *)malloc(128 * sizeof(char));
                sprintf(ssid_str, "Reconnecting to: %s", wifi_config.sta.ssid);
                ui_wifi_setting_label_state_change(WIFI_RECONNECT, ssid_str);
                free(ssid_str);
            }
            if ((change_AP == true) && ((wifi_config.sta.ssid != old_ssid) || (wifi_config.sta.password != old_password) || (wifi_config.sta.bssid != old_bssid)))
            {
                if (attemp_count < WIFI_connect_max_attemp_number)
                {
                    esp_wifi_connect();
                    attemp_count += 1;
                    ESP_LOGE(__func__, "Wi-Fi disconnected: Retrying connect to AP SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
                }
                else
                {
                    memcpy(wifi_config.sta.ssid, old_ssid, sizeof(old_ssid));
                    memcpy(wifi_config.sta.password, old_password, sizeof(old_password));
                    ESP_LOGI(TAG, "Reconfigurating the previous AP: SSID: %s PASSWORDS: %s", wifi_config.sta.ssid, wifi_config.sta.password);
                    memcpy(wifi_config.sta.bssid, old_bssid, sizeof(old_bssid));
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                    attemp_count += 1;
                }
                ESP_LOGI(TAG, "Connect to the new AP unsuccessfully");
            }
            else
            {
                if (change_AP == true)
                {
                    esp_wifi_disconnect();
                    change_AP = false;
                }
            }
            if ((change_AP == false) || (attemp_count > WIFI_connect_max_attemp_number))
            {
                if (change_AP == true)
                {
                    ESP_LOGI(TAG, "Trying to reconnect to the previous AP");
                    change_AP = false;
                }

                if (attemp_count_1 < WIFI_connect_max_attemp_number)
                {
                    esp_wifi_connect();
                    attemp_count_1 += 1;
                    ESP_LOGE(TAG, "Wi-Fi disconnected: Retrying connect to AP SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
                }
                else
                {
                    ESP_LOGI(TAG, "Connect to the AP unsuccessfully. Smart Config again");
                    if (esp_smartconfig_stop() == ESP_OK)
                    {
                        esp_smartconfig_start(&smart_cfg);
                    }
                }
            }

            break;
        }
        default:
            break;
        }
    }
    else
    {
        if (event_base == IP_EVENT)
        {
            if (event_id == IP_EVENT_STA_GOT_IP)
            {
                // wifi_state_display(true);
                ESP_LOGI(TAG, "The Device got an IP successfully");
                attemp_count = 0;
                attemp_count_1 = 0;
                is_connected = true;
                memcpy(old_ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
                memcpy(old_password, wifi_config.sta.password, sizeof(wifi_config.sta.password));
                memcpy(old_bssid, wifi_config.sta.bssid, sizeof(wifi_config.sta.bssid));
                start_file_server(&file_server, base_path); // start file server
                wifi_ap_record_t ap_info;
                esp_wifi_sta_get_ap_info(&ap_info);
                char *ssid_str = (char *)malloc(128 * sizeof(char));
                sprintf(ssid_str, "Connected to: %s", wifi_config.sta.ssid);
                ui_wifi_setting_label_state_change(WIFI_GOT_IP, ssid_str);
                free(ssid_str);
                ESP_LOGI(TAG, "AP MAC ADDRESS: %02X:%02X:%02X:%02X:%02X:%02X", ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2], ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
                ui_update_device_icon_state(wifi_icon, true);
            }
        }
        else
        {
            if (event_base == SC_EVENT)
            {
                switch (event_id)
                {
                case SC_EVENT_SCAN_DONE:
                {
                    ui_wifi_setting_label_state_change(WIFI_SCAN_DONE, "Finding Access Point...");
                    ESP_LOGI(SC_TAG, "Scan done");
                    break;
                }
                case SC_EVENT_FOUND_CHANNEL:
                {
                    attemp_count = 0;
                    attemp_count_1 = 0;
                    change_AP = true;
                    ui_wifi_setting_label_state_change(WIFI_FOUND_CHANNEL, "Found Access Point");
                    ESP_LOGI(SC_TAG, "Found channel");
                    break;
                }
                case SC_EVENT_GOT_SSID_PSWD:
                {
                    ESP_LOGI(SC_TAG, "Got SSID and password");

                    smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;

                    uint8_t ssid[33] = {0};
                    uint8_t password[65] = {0};
                    uint8_t bssid[6] = {0};
                    uint8_t rvd_data[33] = {0};

                    bzero(&wifi_config, sizeof(wifi_config_t));
                    memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
                    memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
                    wifi_config.sta.bssid_set = evt->bssid_set;
                    if (wifi_config.sta.bssid_set == true)
                    {
                        memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
                        memcpy(bssid, evt->bssid, sizeof(evt->bssid));
                        ESP_LOGI(SC_TAG, "AP MAC ADDRESS: %02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
                    }

                    memcpy(ssid, evt->ssid, sizeof(evt->ssid));
                    memcpy(password, evt->password, sizeof(evt->password));

                    ESP_LOGI(SC_TAG, "SSID:%s", ssid);
                    ESP_LOGI(SC_TAG, "PASSWORD:%s", password);
                    char *ssid_str = (char *)malloc(128 * sizeof(char));
                    sprintf(ssid_str, "Connecting to: %s", ssid);
                    ui_wifi_setting_label_state_change(WIFI_GOT_SSID_PWD, ssid_str);
                    free(ssid_str);
                    if (evt->type == SC_TYPE_ESPTOUCH_V2)
                    {
                        ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
                        ESP_LOGI(SC_TAG, "RVD_DATA:");
                        for (int i = 0; i < 33; i++)
                        {
                            printf("%02x ", rvd_data[i]);
                        }
                        printf("\n");
                    }

                    ESP_ERROR_CHECK(esp_wifi_disconnect());
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                    esp_wifi_connect();
                    break;
                }
                case SC_EVENT_SEND_ACK_DONE:
                {
                    esp_smartconfig_stop();
                    change_AP = false;

                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

/**
 *
 * @brief This function initialize wifi and create, start WiFi handle such as loop (low priority)
 *
 */
static void WIFI_init(void)
{
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    esp_event_handler_instance_t instance_any_id_Wifi;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_t instance_any_id_SmartConfig;

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_event_handler, &instance_any_id_Wifi));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi_event_handler, &instance_got_ip));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &Wifi_event_handler, &instance_any_id_SmartConfig));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
}

/*------------------------------------ GET DATA FROM SENSOR ------------------------------------ */

void IRAM_ATTR clean_chamber_Timer_ISR_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(stop_clean_chamber_semaphore, xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    gptimer_stop(clean_chamber_Timer);
}

/**
 * @brief Task to continuously read temperature and humidity from the sensor
 *        and update the UI with the latest values.
 *
 * This task runs indefinitely, waiting for access to the temperature and
 * humidity monitor semaphore. Once access is granted, it measures the
 * temperature and humidity using the SHT3x sensor and updates the UI with
 * these values. The semaphore is then released, and the task delays for 2
 * seconds before attempting to read the sensor again.
 *
 * @param parameters Pointer to parameters passed to the task (unused).
 */

void readSenorChamberTemperature_task(void *parameters)
{
    struct dataSensor_st dataSensorTemp = {0};

    for (;;)
    {
        if (xSemaphoreTake(monitor_temperature_humidity_semaphore, portMAX_DELAY) == pdTRUE)
        {
            sht3x_measure(&sht30_sensor, &dataSensorTemp.temperature, &dataSensorTemp.humidity);
            ui_update_temperature_humidity(dataSensorTemp.temperature, dataSensorTemp.humidity);
            ESP_LOGI("HEAP", "Free heap size: %" PRIu32 " bytes", esp_get_free_heap_size());
            xSemaphoreGive(monitor_temperature_humidity_semaphore);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}

void sampling_process_task(void *parameters)
{
    struct dataSensor_st dataSensorTemp = {0};
    TickType_t task_lastWakeTime;
    TickType_t finishTime;
    measure_control_eventGroup = xEventGroupCreate(); // create event group for control measurement process

    getDataSensor_semaphore = xSemaphoreCreateMutex();
    monitor_temperature_humidity_semaphore = xSemaphoreCreateMutex();
    stop_clean_chamber_semaphore = xSemaphoreCreateBinary(); // create semaphore for stop cleaning sensor chamber stage in measurement process
    // Initialize timer for sensor chamber cleaning stage timer
    gptimer_config_t clean_chamber_Timer_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick= 1/resolution = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&clean_chamber_Timer_cfg, &clean_chamber_Timer));

    // Resgister callback for sensor chamber cleaning stage timer
    gptimer_event_callbacks_t clean_chamber_cbs = {
        .on_alarm = clean_chamber_Timer_ISR_handler,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(clean_chamber_Timer, &clean_chamber_cbs, NULL));
    ESP_LOGI(TAG, "Enable clean chamber timer");
    ESP_ERROR_CHECK(gptimer_enable(clean_chamber_Timer));
    // Set alarm config for sensor chamber cleaning timer
    gptimer_alarm_config_t clean_chamber_alarm_config = {
        .reload_count = 0,                                                         // reload count value (the start value when starting timer)
        .alarm_count = CLEAN_CHAMBER_TIME * clean_chamber_Timer_cfg.resolution_hz, // alarm count value (the end value of timer before the alarm occurs)
        .flags.auto_reload_on_alarm = true,                                        // auto reload timer to reload count value when alarm occur
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(clean_chamber_Timer, &clean_chamber_alarm_config));

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcf8575_init_desc(&pcf8575_device, CONFIG_PCF8575_I2C_ADDRESS, CONFIG_PCF8575_I2C_PORT, CONFIG_PCF8575_PIN_NUM_SDA, CONFIG_PCF8575_PIN_NUM_SCL, (-1), NULL));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcf8575_pin_write(&pcf8575_device, PCF8575_GPIO_PIN_17, 1));
    // ESP_ERROR_CHECK_WITHOUT_ABORT(pcf8575_pin_write(&pcf8575_device, PCF8575_GPIO_PIN_13, 1));
    gpio_set_level(RELAY_TRIGGER_PIN, 0);
    ESP_ERROR_CHECK_WITHOUT_ABORT(sht3x_init_desc(&sht30_sensor, 0x44, 1, 21, 22));
    moduleError.sht85Error = ESP_ERROR_CHECK_WITHOUT_ABORT(sht3x_init(&sht30_sensor));
    vTaskDelay(sht3x_get_measurement_duration(SHT3X_HIGH));
    // Start periodic measurements with 1 measurement per second.
    ESP_ERROR_CHECK_WITHOUT_ABORT(sht3x_start_measurement(&sht30_sensor, SHT3X_PERIODIC_1MPS, SHT3X_HIGH));
    // Wait until first measurement is ready (constant time of at least 30 ms
    // or the duration returned from *sht3x_get_measurement_duration*).
    vTaskDelay(sht3x_get_measurement_duration(SHT3X_HIGH));
    if (moduleError.sht85Error == ESP_OK)
    {
        ui_update_device_icon_state(temp_sensor_icon, true);
    }
    else
    {
        ui_error_notify_screen();
    }
    // Setup for ADS1115
    moduleError.isads1115Error = true;
    memset(ads111x_devices, 0, sizeof(ads111x_devices));
    for (size_t i = 0; i < CONFIG_ADS111X_DEVICE_COUNT; i++)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(ads111x_init_desc(&ads111x_devices[i], addresses[i], CONFIG_ADS111X_I2C_PORT, CONFIG_ADS111X_I2C_MASTER_SDA, CONFIG_ADS111X_I2C_MASTER_SCL));
        moduleError.ads1115_1Error = ESP_ERROR_CHECK_WITHOUT_ABORT(ads111x_set_mode(&ads111x_devices[i], ADS111X_MODE_CONTINUOUS));    // Continuous conversion mode
        moduleError.ads1115_2Error = ESP_ERROR_CHECK_WITHOUT_ABORT(ads111x_set_data_rate(&ads111x_devices[i], ADS111X_DATA_RATE_475)); // 128 samples per second
        moduleError.ads1115_3Error = ESP_ERROR_CHECK_WITHOUT_ABORT(ads111x_set_gain(&ads111x_devices[i], ads111x_gain_values[ADS111X_GAIN_2V048]));
        if (moduleError.ads1115_1Error != ESP_OK || moduleError.ads1115_2Error != ESP_OK || moduleError.ads1115_3Error != ESP_OK)
        {
            if (moduleError.isads1115Error == true)
            {
                moduleError.isads1115Error = false;
            }
        }
    }
    if (moduleError.isads1115Error == true)
    {
        ui_update_device_icon_state(adc_icon, true);
    }
    if (lv_obj_is_valid(ui_continueLabel) == true)
    {
        start_continue_Animation(ui_continueLabel, 4000); // Ready to switch to main screen
    }
    for (;;)
    {
        EventBits_t bits = xEventGroupWaitBits(measure_control_eventGroup, MEASURE_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        if (bits & MEASURE_BIT)
        {
            ESP_LOGI(__func__, "Measurement process start.");
        }
        if (xSemaphoreTake(monitor_temperature_humidity_semaphore, portMAX_DELAY) == pdTRUE)
        {

            ESP_ERROR_CHECK_WITHOUT_ABORT(ds3231_convertTimeToString(&ds3231_device, nameFileSaveData, 14));
            ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_writeDataToFile(nameFileSaveData, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", "TimeStamp", "Temperature", "Humidity", "EtOH1", "EtOH2", "VOC1", "VOC2", "H2S", "CO", "Odor", "NH3"));
            //  Start cleaning sensor chamber
            if (gpio_get_level(RELAY_TRIGGER_PIN) == 0)
            {
                gpio_set_level(RELAY_TRIGGER_PIN, 1); // Turn on relay to turn off fan
                ui_synchronize_pump_state(true);
            }

            // ESP_ERROR_CHECK(gptimer_start(clean_chamber_Timer));
            // ESP_ERROR_CHECK_WITHOUT_ABORT(pcf8575_pin_write(&pcf8575_device, PCF8575_GPIO_PIN_13, 1)); // Turn on fan
            // ESP_LOGI(__func__, "Start cleaning sensor chamber.");

            // if (xSemaphoreTake(stop_clean_chamber_semaphore, portMAX_DELAY) == pdTRUE)
            // {
            //     ESP_LOGI(__func__, "Stop cleaning sensor chamber. Start sampling stage.");
            //     ui_begin_sampling_stage();
            // }

            // gpio_set_level(RELAY_TRIGGER_PIN, 0); // Turn off relay to turn off fan
            // ui_synchronize_pump_state(false);

            finishTime = xTaskGetTickCount() + SAMPLING_TIME;
            do
            {
                task_lastWakeTime = xTaskGetTickCount();
                dataSensorTemp.timeStamp = 0;
                if (xSemaphoreTake(getDataSensor_semaphore, portMAX_DELAY))
                {
                    ESP_ERROR_CHECK_WITHOUT_ABORT(ds3231_getEpochTime(&ds3231_device, &(dataSensorTemp.timeStamp)));

                    ESP_ERROR_CHECK_WITHOUT_ABORT(sht3x_measure(&sht30_sensor, &dataSensorTemp.temperature, &dataSensorTemp.humidity));
                    ESP_LOGI(__func__, "Temperature: %f, Humidity: %f", dataSensorTemp.temperature, dataSensorTemp.humidity);
                    ui_update_temperature_humidity(dataSensorTemp.temperature, dataSensorTemp.humidity);

                    for (size_t i = 0; i < 4; i++)
                    {
                        for (size_t n = 0; n < 2; n++)
                        {
                            ESP_ERROR_CHECK_WITHOUT_ABORT(ads111x_set_input_mux(&ads111x_devices[n], (ads111x_mux_t)(i + 4)));
                            vTaskDelay(35 / portTICK_PERIOD_MS);
                            int16_t ADC_rawData = 0;
                            if (ads111x_get_value(&ads111x_devices[n], &ADC_rawData) == ESP_OK)
                            {
                                float voltage = (ads111x_gain_values[ADS111X_GAIN_2V048] / ADS111X_MAX_VALUE) * ADC_rawData;
                                ESP_LOGI(__func__, "Raw ADC value: %d, Voltage: %.04f Volts.", ADC_rawData, voltage);
                                dataSensorTemp.ADC_Value[n * 4 + i] = ADC_rawData;
                            }
                            else
                            {
                                ESP_LOGE(__func__, "[%u] Cannot read ADC value.", n);
                            }
                        }
                    }
                    xSemaphoreGive(getDataSensor_semaphore); // Give mutex
                    ESP_LOGI(__func__, "Read data from sensors completed!");

                    if (xQueueSendToBack(dataSensorSentToSD_queue, (void *)&dataSensorTemp, WAIT_10_TICK * 10) != pdPASS)
                    {
                        ESP_LOGE(__func__, "Failed to post the data sensor to dataSensorMidleware Queue.");
                    }
                    else
                    {
                        ESP_LOGI(__func__, "Success to post the data sensor to dataSensorMidleware Queue.");
                    }
                };
                memset(&dataSensorTemp, 0, sizeof(struct dataSensor_st));
                vTaskDelayUntil(&task_lastWakeTime, PERIOD_GET_DATA_FROM_SENSOR);

            } while (task_lastWakeTime < finishTime);
            ui_reset_before_measure_state();
            ui_show_measurement_result(nameFileSaveData);
            // ESP_ERROR_CHECK_WITHOUT_ABORT(pcf8575_pin_write(&pcf8575_device, PCF8575_GPIO_PIN_12, 0)); // Turn off fan
            // gpio_set_level(RELAY_TRIGGER_PIN, 0); // Turn off relay to turn off fan
            gpio_set_level(RELAY_TRIGGER_PIN, 0); // Turn off relay to turn off fan
            ui_synchronize_pump_state(false);
            ESP_LOGI(__func__, "Stop measurement process. Start data analysis process");
            xSemaphoreGive(monitor_temperature_humidity_semaphore);
        }
    }
}

/*------------------------------------ SAVE DATA ------------------------------------ */
/**
 * @brief Save data from SD queue to SD card
 *
 * @param parameters
 */
void saveSensorDataToSDcard_task(void *parameters)
{
    UBaseType_t message_stored = 0;
    struct dataSensor_st dataSensorReceiveFromQueue;

    for (;;)
    {
        message_stored = uxQueueMessagesWaiting(dataSensorSentToSD_queue);

        if (message_stored != 0) // Check if dataSensorSentToSD_queue not empty
        {
            if (xQueueReceive(dataSensorSentToSD_queue, (void *)&dataSensorReceiveFromQueue, WAIT_10_TICK * 50) == pdPASS) // Get data sesor from queue
            {
                ESP_LOGI(__func__, "Receiving data from queue successfully.");

                if (xSemaphoreTake(SDcard_semaphore, portMAX_DELAY) == pdTRUE)
                {
                    static esp_err_t errorCode_t;
                    // Create data string follow format

                    errorCode_t = sdcard_writeDataToFile(nameFileSaveData, dataSensor_templateSaveToSDCard,
                                                         dataSensorReceiveFromQueue.timeStamp,
                                                         dataSensorReceiveFromQueue.temperature,
                                                         dataSensorReceiveFromQueue.humidity,
                                                         dataSensorReceiveFromQueue.ADC_Value[0],
                                                         dataSensorReceiveFromQueue.ADC_Value[1],
                                                         dataSensorReceiveFromQueue.ADC_Value[2],
                                                         dataSensorReceiveFromQueue.ADC_Value[3],
                                                         dataSensorReceiveFromQueue.ADC_Value[4],
                                                         dataSensorReceiveFromQueue.ADC_Value[5],
                                                         dataSensorReceiveFromQueue.ADC_Value[6],
                                                         dataSensorReceiveFromQueue.ADC_Value[7]);
                    ESP_LOGI(TAG, "Save task received mutex!");
                    xSemaphoreGive(SDcard_semaphore);
                    if (errorCode_t != ESP_OK)
                    {
                        ESP_LOGE(__func__, "sdcard_writeDataToFile(...) function returned error: 0x%.4X", errorCode_t);
                    }
                }
            }
            else
            {
                ESP_LOGI(__func__, "Receiving data from queue failed.");
                continue;
            }
        }

        vTaskDelay(PERIOD_SAVE_DATA_SENSOR_TO_SDCARD);
    }
};

/*****************************************************************************************************/
/*-------------------------------  MAIN_APP DEFINE FUNCTIONS  ---------------------------------------*/
/*****************************************************************************************************/

static void initialize_nvs(void)
{
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
        error = nvs_flash_init();
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(error);
}

void app_main(void)
{
    // Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(200));
    ESP_LOGI(__func__, "Starting app main.");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK)
    {
        printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
               (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    }
    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    ESP_LOGI(__func__, "Name device: %s.", CONFIG_NAME_DEVICE);

    // Initialize nvs partition
    ESP_LOGI(__func__, "Initialize nvs partition.");
    initialize_nvs();
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());
    // Wait a second for memory initialization
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // GPIO for control relay 5V (NC - high level trigger)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_TRIGGER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);

#if (CONFIG_USING_SDCARD)
    // Initialize SPI Bus
    ESP_LOGI(__func__, "Initialize SD card with SPI interface.");
    esp_vfs_fat_mount_config_t mount_config_t = MOUNT_CONFIG_DEFAULT();
    spi_bus_config_t spi_bus_config_t = SPI_BUS_CONFIG_DEFAULT();
    sdmmc_host_t host_t = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_PIN_NUM_CS;
    slot_config.host_id = host_t.slot;

    sdmmc_card_t SDCARD;
    moduleError.sdError = sdcard_initialize(&mount_config_t, &SDCARD, &host_t, &spi_bus_config_t, &slot_config);
    ESP_ERROR_CHECK_WITHOUT_ABORT(moduleError.sdError);
    SDcard_semaphore = xSemaphoreCreateMutex();
#endif // CONFIG_USING_SDCARD

#if CONFIG_USING_LCD_TFT
    ili9341_init();
    xTaskCreate(&lvgl_timer_handle_task, "LVGL timer handle task", 15 * 1024, NULL, 18, NULL);
    ui_init();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    if (moduleError.sdError == ESP_OK)
    {
        ui_update_device_icon_state(sdcard_icon, true);
    }
    else
    {
        ui_error_notify_screen();
    }
#endif // CONFIG_USING_LCD_TFT

    ESP_ERROR_CHECK_WITHOUT_ABORT(i2cdev_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(ds3231_initialize(&ds3231_device, CONFIG_RTC_I2C_PORT, CONFIG_RTC_PIN_NUM_SDA, CONFIG_RTC_PIN_NUM_SCL));

    // Create dataSensorQueue
    dataSensorSentToSD_queue = xQueueCreate(QUEUE_SIZE, sizeof(struct dataSensor_st));
    while (dataSensorSentToSD_queue == NULL)
    {
        ESP_LOGE(__func__, "Create dataSensorSentToSD Queue failed.");
        ESP_LOGI(__func__, "Retry to create dataSensorSentToSD Queue...");
        vTaskDelay(500 / portTICK_PERIOD_MS);
        dataSensorSentToSD_queue = xQueueCreate(QUEUE_SIZE, sizeof(struct dataSensor_st));
    };
    ESP_LOGI(__func__, "Create dataSensorSentToSD Queue success.");

    xTaskCreate(&sampling_process_task, "Sampling task", (1024 * 33), NULL, (UBaseType_t)25, &getDataFromSensorTask_handle);

    xTaskCreate(&saveSensorDataToSDcard_task, "Save Sensor Data To SDcard task", (1024 * 16), NULL, (UBaseType_t)19, &saveDataSensorToSDcardTask_handle);

    xTaskCreate(&readSenorChamberTemperature_task, "Temperature monitor task", 10 * 1024, NULL, 11, NULL);

    xTaskCreate(&wifi_control_task, "wifi control task", (10 * 1024), NULL, 10, NULL);

#if CONFIG_USING_WIFI
    WIFI_init();
#endif
}
