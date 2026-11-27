#ifndef PELTIER_MESH_H
#define PELTIER_MESH_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    float temp_c;
    float humidity;
    float hot_c;
    uint32_t errors;
    bool enabled;
    bool output_on;
} peltier_mesh_status_t;

esp_err_t peltier_mesh_start(bool root);
bool peltier_mesh_node_enabled(void);
void peltier_mesh_send_status(const peltier_mesh_status_t *status);

#endif
