#ifndef PELTIER_MESH_H
#define PELTIER_MESH_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    int16_t temp_low, temp_high;
    int16_t humidity_low, humidity_high;
} peltier_limits_t;  /* tenths of a degree C or percent RH */

typedef enum {
    PELTIER_LIMIT_IDLE, PELTIER_LIMIT_PENDING, PELTIER_LIMIT_APPLIED,
    PELTIER_LIMIT_REJECTED, PELTIER_LIMIT_STORAGE_ERROR, PELTIER_LIMIT_TIMEOUT
} peltier_limit_result_t;

typedef struct {
    bool online;
    uint32_t request_id;
    peltier_limit_result_t result;
    peltier_limits_t limits;
} peltier_config_status_t;

static inline bool peltier_limits_valid(const peltier_limits_t *limits)
{
    return limits->temp_low >= -400 && limits->temp_high <= 1250 &&
           limits->temp_low < limits->temp_high &&
           limits->humidity_low >= 0 && limits->humidity_high <= 1000 &&
           limits->humidity_low < limits->humidity_high;
}

typedef struct {
    float temp_c;
    float humidity;
    float hot_c;
    uint32_t errors;
    bool enabled;
    bool output_on;
    peltier_limits_t limits;
} peltier_mesh_status_t;

esp_err_t peltier_mesh_start(bool root);
bool peltier_mesh_node_enabled(peltier_limit_result_t (*apply)(const peltier_limits_t *));
void peltier_mesh_get_config(peltier_config_status_t *status);
esp_err_t peltier_mesh_set_config(const peltier_limits_t *limits, uint32_t *request_id);
void peltier_mesh_send_status(const peltier_mesh_status_t *status);

#endif
