#ifndef TELEMETRY_LOGGER_H
#define TELEMETRY_LOGGER_H

#include <stdbool.h>
#include "esp_err.h"
#include "peltier_fsm.h"

esp_err_t telemetry_logger_init(void);
void telemetry_log_uart(const peltier_fsm_state_t *fsm);
void telemetry_send_can(const peltier_fsm_state_t *fsm);
bool telemetry_check_can_alive(void);

#endif // TELEMETRY_LOGGER_H