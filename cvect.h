#ifndef CVECT_H
#define CVECT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FREE
#define FREE(x)                                                                \
  do {                                                                         \
    free((x));                                                                 \
    (x) = NULL;                                                                \
  } while (0)
#endif

typedef struct {
  size_t c, n, s;
  void *v;
} cvec;

/*
 * Initialize a vector of initial capacity `c`,
 * whose members have size of `size`
 *
 * Returns: `cvect`
 */
[[nodiscard]] static inline cvec cvect_init(size_t size, size_t c) {
  return (cvec){.c = c, .n = 0, .s = size, .v = malloc(size * c)};
}

static inline size_t cvect_expand_(cvec *restrict vec) {
  void *tmp = realloc(vec->v, vec->s * (vec->c << 1));
  if (!tmp)
    return 0;
  vec->v = tmp;
  vec->c <<= 1;
  return vec->c;
}

/*
 * Delete the element of vector `*vec` at position `i`
 */
static inline void cvect_d(cvec *restrict vec, const size_t i) {
  if (i > vec->n)
    return;
  vec->n--;
  if (i == vec->n)
    return;
  memmove(&((unsigned char *)vec->v)[i * vec->s],
          &((unsigned char *)vec->v)[(i + 1) * vec->s], (vec->n - i) * vec->s);
}

/*
 * Destroy the vector `vec`
 *
 * NOTE: this only frees the container.
 * It is delegated to the user to delete the content, if necessary
 */
static inline void cvect_destroy(cvec *restrict vec) { FREE(vec->v); }

/*
 * Append the object `x` of type `type` to the cvect `vec`
 *
 * NOTE: function-like inline macro
 */
#define cvect_a(vec, type, x)                                                  \
  do {                                                                         \
    if ((vec).n >= (vec).c)                                                    \
      cvect_expand_(&(vec));                                                   \
    ((type *)(vec).v)[(vec).n] = (x);                                          \
    (vec).n++;                                                                 \
  } while (0)

/*
 * Insert the object `x` of type `type` to the cvect `vec` at position `i`
 *
 * NOTE: function-like inline macro
 */
#define cvect_i(vec, type, x, i)                                               \
  do {                                                                         \
    if (i > (vec).c)                                                           \
      cvect_a((vec), type, x);                                                 \
    if ((vec).n >= (vec).c)                                                    \
      cvect_expand_(&(vec));                                                   \
    memmove(&((type *)(vec).v)[i + 1], &((type *)(vec).v)[i],                  \
            ((vec).n - i) * (vec).s);                                          \
    ((type *)(vec).v)[i] = x;                                                  \
    (vec).n++;                                                                 \
  } while (0)

/*
 * Get the element of type `type` to the cvec `vec` at position `i`
 *
 * NOTE: inline macro
 */
#define cvect_g(vec, type, i) ((type *)(vec).v)[i]

#endif
