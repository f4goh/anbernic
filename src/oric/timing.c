#include "timing.h"

#include <sched.h>


absolute_time_t get_absolute_time(void)
{
    absolute_time_t t;

    clock_gettime(
        CLOCK_MONOTONIC,
        &t
    );

    return t;
}


absolute_time_t delayed_by_us(
    absolute_time_t t,
    int64_t us
)
{
    t.tv_nsec += us * 1000;

    while (t.tv_nsec >= 1000000000L)
    {
        t.tv_nsec -= 1000000000L;
        t.tv_sec++;
    }

    return t;
}


int64_t to_us_since_boot(
    absolute_time_t t
)
{
    return ((int64_t)t.tv_sec * 1000000LL)
           +
           (t.tv_nsec / 1000);
}


bool time_reached(
    absolute_time_t t
)
{
    absolute_time_t now =
        get_absolute_time();

    return to_us_since_boot(now)
           >=
           to_us_since_boot(t);
}


void busy_wait_until(absolute_time_t t)
{
    while (!time_reached(t))
    {
        // rien
    }
}
