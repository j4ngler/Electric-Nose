#ifndef PELTIER_AOD4184_H
#define PELTIER_AOD4184_H

#include <stdbool.h>
#include "esp_err.h"

esp_err_t peltier_aod4184_init(void);
void peltier_aod4184_set_state(bool enable);
bool peltier_aod4184_get_state(void);

#endif // PELTIER_AOD4184_H