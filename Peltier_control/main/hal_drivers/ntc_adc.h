#ifndef NTC_ADC_H
#define NTC_ADC_H

#include "esp_err.h"

esp_err_t ntc_adc_init(void);
esp_err_t ntc_adc_read_temp(float *temp_hot_c);

#endif // NTC_ADC_H