#include "tracing.h"

struct timeval timeval_subtract(struct timeval *x, struct timeval *y) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }
  return (struct timeval){.tv_sec = x->tv_sec - y->tv_sec,
                          .tv_usec = x->tv_usec - y->tv_usec};
}

void timeval_add(struct timeval *x, struct timeval *y) {
  x->tv_sec += y->tv_sec;
  x->tv_usec += y->tv_usec;
  if (x->tv_usec >= 1000000) {
    x->tv_sec++;
    x->tv_usec -= 1000000;
  } else if (x->tv_usec <= -1000000) {
    x->tv_sec--;
    x->tv_usec += 1000000;
  }
}

trace_t trace_start() {
  clock_t c = clock();
  struct timeval t;
  gettimeofday(&t, NULL);
  return (trace_t){
      .c = c,
      .t = t,
  };
}

void trace_incnow(trace_t *dest, trace_t *start) {
  trace_t now = trace_start();
  struct timeval tdiff = timeval_subtract(&now.t, &start->t);
  timeval_add(&dest->t, &tdiff);
  dest->c += now.c - start->c;
}

void printtrace_diffnow(trace_t *t, const char *name) {
  struct timeval t1, d;
  clock_t c1 = clock();
  gettimeofday(&t1, NULL);
  d = timeval_subtract(&t1, &t->t);
  fprintf(stderr, "[%s] time: %8ld:%04ld s:us\t CPU: %8ld s\n", name, d.tv_sec,
          d.tv_usec, (c1 - t->c) / CLOCKS_PER_SEC);
}

void printtrace(trace_t *t, const char *name) {
  fprintf(stderr, "[%s] time: %8ld:%04ld s:us\t CPU: %8ld s\n", name,
          t->t.tv_sec, t->t.tv_usec, t->c / CLOCKS_PER_SEC);
}
