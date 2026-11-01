#ifndef SHT35_AO3401_H
#define SHT35_AO3401_H

#include "esp_err.h"

esp_err_t sht35_ao3401_init(void);
esp_err_t sht35_ao3401_read_temp_hum(float *temp_c, float *hum_rh);
void sht35_ao3401_power_off_bus_float(void);

#endif // SHT35_AO3401_H