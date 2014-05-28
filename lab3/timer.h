#ifndef TIMER_H
#define TIMER_H

struct timespec t0, t1; /* Start/end time for timer */

/* Timer macros */
#define TIMER_START() clock_gettime(CLOCK_MONOTONIC, &t0)
#define TIMER_STOP() clock_gettime(CLOCK_MONOTONIC, &t1)
#define TIMER_ELAPSED_NS() \
(t1.tv_sec * 1000000000 + t1.tv_nsec) - \
(t0.tv_sec * 1000000000 + t0.tv_nsec)
#define TIMER_ELAPSED_US() \
(t1.tv_sec * 1000000 + (double)t1.tv_nsec / 1000) - \
(t0.tv_sec * 1000000 + (double)t0.tv_nsec / 1000)

#endif /* TIMER_H */