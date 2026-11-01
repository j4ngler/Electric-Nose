#include "moving_avg_filter.h"
#include <string.h>

void moving_avg_init(moving_avg_filter_t *filter, size_t window_size)
{
    if (!filter) return;
    if (window_size < 1) window_size = 1;
    if (window_size > MOVING_AVG_MAX_WINDOW) window_size = MOVING_AVG_MAX_WINDOW;

    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->window_size = window_size;
    filter->count = 0;
    filter->index = 0;
    filter->sum = 0.0f;
}

void moving_avg_reset(moving_avg_filter_t *filter)
{
    if (!filter) return;
    moving_avg_init(filter, filter->window_size);
}

float moving_avg_update(moving_avg_filter_t *filter, float sample)
{
    if (!filter) return sample;

    if (filter->count == filter->window_size) {
        filter->sum -= filter->buffer[filter->index];
    } else {
        filter->count++;
    }

    filter->buffer[filter->index] = sample;
    filter->sum += sample;
    filter->index = (filter->index + 1) % filter->window_size;

    return filter->sum / (float)filter->count;
}