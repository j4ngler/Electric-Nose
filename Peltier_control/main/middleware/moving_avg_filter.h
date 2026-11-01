#ifndef MOVING_AVG_FILTER_H
#define MOVING_AVG_FILTER_H

#include <stddef.h>
#include <stdbool.h>

#define MOVING_AVG_MAX_WINDOW   10

typedef struct {
    float buffer[MOVING_AVG_MAX_WINDOW];
    size_t window_size;
    size_t count;
    size_t index;
    float sum;
} moving_avg_filter_t;

void moving_avg_init(moving_avg_filter_t *filter, size_t window_size);
void moving_avg_reset(moving_avg_filter_t *filter);
float moving_avg_update(moving_avg_filter_t *filter, float sample);

#endif // MOVING_AVG_FILTER_H