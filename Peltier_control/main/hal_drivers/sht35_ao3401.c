#include "sht35_ao3401.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define SHT35_I2C_PORT          I2C_NUM_0
#define SHT35_I2C_ADDR          0x45    // Standard address of DFRobot SHT35
#define SHT35_I2C_FREQ_HZ       100000
#define AO3401_ON_LEVEL         0       // P-MOSFET AO3401 conducts when Gate = LOW
#define AO3401_OFF_LEVEL        1       // P-MOSFET AO3401 turns off when Gate = HIGH

static const char *TAG = "SHT35_AO3401";

static uint8_t sht35_crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x31) : (crc << 1);
        }
    }
    return crc;
}

void sht35_ao3401_power_off_bus_float(void)
{
    // Set all the pins connected to JP 1x6 (SDA, SCL, RST, INT) to INPUT floating (turn off pull-up/down)
    uint64_t sensor_pins_mask = (1ULL << CONFIG_PELTIER_I2C_SDA_GPIO) |
                                (1ULL << CONFIG_PELTIER_I2C_SCL_GPIO) |
                                (1ULL << CONFIG_PELTIER_SHT35_RST_GPIO) |
                                (1ULL << CONFIG_PELTIER_SHT35_INT_GPIO);

    gpio_config_t io_conf = {
        .pin_bit_mask = sensor_pins_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Turn off power to 3.3V via AO3401 (Gate = HIGH)
    gpio_set_level(CONFIG_PELTIER_AO3401_GPIO, AO3401_OFF_LEVEL);
}

esp_err_t sht35_ao3401_init(void)
{
    gpio_set_level(CONFIG_PELTIER_AO3401_GPIO, AO3401_OFF_LEVEL);

    gpio_config_t pwr_conf = {
        .pin_bit_mask = (1ULL << CONFIG_PELTIER_AO3401_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t err = gpio_config(&pwr_conf);
    if (err != ESP_OK) return err;

    sht35_ao3401_power_off_bus_float();
    ESP_LOGI(TAG, "Initialized AO3401 (GPIO%d) & DFRobot SHT35 (Addr=0x%02X, SDA=%d, SCL=%d, RST=%d, INT=%d) in High-Z mode.",
             CONFIG_PELTIER_AO3401_GPIO,
             SHT35_I2C_ADDR,
             CONFIG_PELTIER_I2C_SDA_GPIO,
             CONFIG_PELTIER_I2C_SCL_GPIO,
             CONFIG_PELTIER_SHT35_RST_GPIO,
             CONFIG_PELTIER_SHT35_INT_GPIO);
    return ESP_OK;
}

esp_err_t sht35_ao3401_read_temp_hum(float *temp_c, float *hum_rh)
{
    if (!temp_c || !hum_rh) return ESP_ERR_INVALID_ARG;

    // 1. Power the SHT35 (SENSOR_3V3_SW) with AO3401 at 3.3V
    gpio_set_level(CONFIG_PELTIER_AO3401_GPIO, AO3401_ON_LEVEL);

    // 2. Pull the RST pin (GPIO23) HIGH to get the SHT35 out of Reset
    gpio_config_t rst_conf = {
        .pin_bit_mask = (1ULL << CONFIG_PELTIER_SHT35_RST_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&rst_conf);
    gpio_set_level(CONFIG_PELTIER_SHT35_RST_GPIO, 1);

    // Wait 15 ms for the filter capacitor on the DFRobot SHT35 module and C11 (104) to charge
    vTaskDelay(pdMS_TO_TICKS(15));

    // 3. Initialize the I2C bus on SDA (GPIO21) / SCL (GPIO22)
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_PELTIER_I2C_SDA_GPIO,
        .scl_io_num = CONFIG_PELTIER_I2C_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = SHT35_I2C_FREQ_HZ,
    };

    esp_err_t err = i2c_param_config(SHT35_I2C_PORT, &conf);
    if (err != ESP_OK) {
        sht35_ao3401_power_off_bus_float();
        return err;
    }

    err = i2c_driver_install(SHT35_I2C_PORT, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        sht35_ao3401_power_off_bus_float();
        return err;
    }

    // 4. Send the Single-Shot High Repeatability measurement command (0x2400) to address 0x45
    uint8_t cmd[2] = {0x24, 0x00};
    err = i2c_master_write_to_device(SHT35_I2C_PORT, SHT35_I2C_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(50));
    if (err == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(18));

        uint8_t raw[6] = {0};
        err = i2c_master_read_from_device(SHT35_I2C_PORT, SHT35_I2C_ADDR, raw, sizeof(raw), pdMS_TO_TICKS(50));
        if (err == ESP_OK) {
            if (sht35_crc8(&raw[0], 2) != raw[2] || sht35_crc8(&raw[3], 2) != raw[5]) {
                ESP_LOGE(TAG, "Error checking CRC-8 from SHT35 sensor!");
                err = ESP_ERR_INVALID_CRC;
            } else {
                uint16_t raw_t = ((uint16_t)raw[0] << 8) | raw[1];
                uint16_t raw_h = ((uint16_t)raw[3] << 8) | raw[4];
                *temp_c = -45.0f + 175.0f * ((float)raw_t / 65535.0f);
                *hum_rh = 100.0f * ((float)raw_h / 65535.0f);
            }
        }
    }

    // 5.Release I2C, set SDA/SCL/RST/INT to INPUT floating and turn off AO3401
    i2c_driver_delete(SHT35_I2C_PORT);
    sht35_ao3401_power_off_bus_float();

    return err;
}