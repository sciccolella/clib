#ifndef TRACING_H
#define TRACING_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/*
 * Add `y` to `x`
 */
void timeval_add(struct timeval *x, struct timeval *y);

struct timeval timeval_subtract(struct timeval *x, struct timeval *y);

typedef struct {
  clock_t c;
  struct timeval t;
} trace_t;

/*
 * Create a new trace containing current time
 */
trace_t trace_start();
/*
 * Increment `dest` trace with the values computed from `now - start`
 */
void trace_incnow(trace_t *dest, trace_t *start);
/*
 * Print the tracing of `t - now` with `name` as recognizer
 */
void printtrace_diffnow(trace_t *t, const char *name);
/*
 * Print the tracing `t`, meant to be used with incremental tracing
 * `trace_inc*()` functions
 */
void printtrace(trace_t *t, const char *name);
/*
 * Trace computation time of function `f`, if the function has
 * a return value, it should be passed in the variadic argument.
 *
 * Examples:
 * void fun_noret(int x, int y);
 * int fun_ret(double z);
 *
 * TRACE(fun_noret(1, 2));
 * int res;
 * TRACE(fun_ret(0.1), res);
 * // res contains the return value of `fun_ret`
 *
 *
 */
#define TRACE(f, ...)                                                          \
  do {                                                                         \
    struct timeval t0, t1, d;                                                  \
    clock_t c0 = clock(), c1;                                                  \
    gettimeofday(&t0, NULL);                                                   \
    __VA_OPT__(__VA_ARGS__ =)(f);                                              \
    gettimeofday(&t1, NULL);                                                   \
    d = timeval_subtract(&t1, &t0);                                            \
    c1 = clock();                                                              \
    fprintf(stderr, "[%s] time: %8ld:%04ld s:us\t CPU: %8ld s\n", "" #f "",    \
            d.tv_sec, d.tv_usec, (c1 - c0) / CLOCKS_PER_SEC);                  \
  } while (0)

#endif // !TRACING_H
