#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef struct timespec absolute_time_t;

absolute_time_t get_absolute_time(void);

absolute_time_t delayed_by_us(
    absolute_time_t t,
    int64_t us
);

int64_t to_us_since_boot(
    absolute_time_t t
);

bool time_reached(
    absolute_time_t t
);

void busy_wait_until(
    absolute_time_t t
);

#endif
