#ifndef PELTIER_FSM_H
#define PELTIER_FSM_H

#include <stdbool.h>
#include <stdint.h>
#include "app_config.h"

typedef enum {
    PELTIER_ERR_NONE            = 0,
    PELTIER_ERR_SHT35_FAULT     = (1 << 0),
    PELTIER_ERR_NTC_FAULT       = (1 << 1),
    PELTIER_ERR_OVERHEAT        = (1 << 2),
    PELTIER_ERR_CAN_TIMEOUT     = (1 << 3),
    PELTIER_ERR_SYS_DISABLED    = (1 << 4)
} peltier_error_flags_t;

typedef struct {
    bool peltier_on;
    bool overheat_latched;
    int64_t last_switch_time_ms;
    uint32_t error_flags;
    float filtered_temp_c;
    float filtered_hum_rh;
    float temp_hot_c;
} peltier_fsm_state_t;

void peltier_fsm_init(peltier_fsm_state_t *fsm);
void peltier_fsm_update(peltier_fsm_state_t *fsm,
                        const app_config_t *cfg,
                        float temp_c,
                        float hum_rh,
                        float temp_hot_c,
                        bool sht35_ok,
                        bool ntc_ok,
                        bool can_ok);

#endif // PELTIER_FSM_H