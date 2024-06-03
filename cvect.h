#ifndef CVECT_H
#define CVECT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREE(x)                                                                \
  do {                                                                         \
    free((x));                                                                 \
    (x) = NULL;                                                                \
  } while (0)

typedef struct {
  size_t c, n, s;
  void *v;
} cvect;

[[nodiscard]] static inline cvect cvect_init(size_t s, size_t c) {
  return (cvect){.c = c, .n = 0, .s = s, .v = malloc(s * c)};
}

static inline size_t cvect_expand_(cvect *restrict vec) {
  void *tmp = realloc(vec->v, vec->s * (vec->c << 1));
  if (!tmp)
    return 0;
  vec->v = tmp;
  vec->c <<= 1;
  return vec->c;
}

static inline void cvect_d(cvect *restrict vec, const size_t i) {
  if (i > vec->n)
    return;
  vec->n--;
  if (i == vec->n)
    return;
  memmove(&((unsigned char *)vec->v)[i * vec->s],
          &((unsigned char *)vec->v)[(i + 1) * vec->s], (vec->n - i) * vec->s);
}

static inline void cvect_destroy(cvect *restrict vec) {
  free(vec->v);
  vec->v = NULL;
}

#define cvect_a(vec, type, x)                                                  \
  do {                                                                         \
    if ((vec)->n >= (vec)->c)                                                  \
      cvec_expand_((vec));                                                     \
    ((type *)(vec)->v)[(vec)->n] = (x);                                        \
    (vec)->n++;                                                                \
                                                                               \
  } while (0)

#define cvect_i(vec, type, x, i)                                               \
  do {                                                                         \
    if (i > (vec)->c)                                                          \
      cvect_a((vec), type, x);                                                 \
    if ((vec)->n >= (vec)->c)                                                  \
      cvec_expand_((vec));                                                     \
    memmove(&((type *)(vec)->v)[i + 1], &((type *)(vec)->v)[i],                \
            ((vec)->n - i) * (vec)->s);                                        \
    ((type *)(vec)->v)[i] = x;                                                 \
  } while (0)

#define cvect_destroy_iter(vec, type)                                          \
  do {                                                                         \
    for (size_t i = 0; i < vec->n; i++)                                        \
      FREE(((type *)(vec)->v)[i]);                                             \
    FREE(vec->v);                                                              \
    /* free(vec->v); */                                                        \
    /* vec->v = NULL; */                                                       \
  } while (0)

/* #define CVEC_DECLAREP(type, typename) */ /* static inline void cvec_## */
/* typename##_destroy_iter(cvec_##typename *restrict vec) { */  /* for (size_t i
                                                                   = 0; i <
                                                                   vec->n; i++)
                                                                 */
/* free(vec->v[i]); */ /* free(vec->v); */ /* vec->v = NULL; */ /* } */

#endif
