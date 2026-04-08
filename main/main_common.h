#ifndef MAIN_H
#define MAIN_H

#include "freertos/event_groups.h"
#include "pcf8575.h"
#include "driver/gpio.h"

extern EventGroupHandle_t wifi_control_eventGroup;
extern EventGroupHandle_t measure_control_eventGroup;
extern i2c_dev_t pcf8575_device;

#define WIFI_OFF_BIT BIT0
#define WIFI_ON_BIT BIT1
#define MEASURE_BIT BIT2
// Define relay trigger pin
#define RELAY_TRIGGER_PIN GPIO_NUM_25

#endif