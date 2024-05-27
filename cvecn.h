#ifndef CVECn_H
#define CVECn_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t c, n, s;
  void *v;
} cvec;

[[nodiscard]] static inline cvec cvec_init(size_t s, size_t c) {
  return (cvec){.c = c, .n = 0, .s = s, .v = malloc(s * c)};
}

static inline size_t cvec_expand_(cvec *restrict vec) {
  void *tmp = realloc(vec->v, vec->s * (vec->c << 1));
  if (!tmp)
    return 0;
  vec->v = tmp;
  vec->c <<= 1;
  return vec->c;
}

static inline void cvec_d(cvec *restrict vec, const size_t i) {
  if (i > vec->n)
    return;
  vec->n--;
  if (i == vec->n)
    return;
  memmove(&((unsigned char *)vec->v)[i * vec->s],
          &((unsigned char *)vec->v)[(i + 1) * vec->s], (vec->n - i) * vec->s);
}

static inline void cvec_destroy(cvec *restrict vec) {
  free(vec->v);
  vec->v = NULL;
}

#define cvec_a(vec, type, x)                                                   \
  do {                                                                         \
    if ((vec)->n >= (vec)->c)                                                  \
      cvec_expand_((vec));                                                     \
    ((type *)(vec)->v)[(vec)->n] = (x);                                        \
    (vec)->n++;                                                                \
                                                                               \
  } while (0)

#define cvec_i(vec, type, x, i)                                                \
  do {                                                                         \
    if (i > (vec)->c)                                                          \
      cvec_a((vec), type, x);                                                  \
    if ((vec)->n >= (vec)->c)                                                  \
      cvec_expand_((vec));                                                     \
    memmove(&((type *)(vec)->v)[i + 1], &((type *)(vec)->v)[i],                \
            ((vec)->n - i) * (vec)->s);                                        \
    ((type *)(vec)->v)[i] = x;                                                 \
  } while (0)

#define CVEC_DECLAREP(type, typename)                                          \
  static inline void cvec_##                                                   \
      typename##_destroy_iter(cvec_##typename *restrict vec) {                 \
    for (size_t i = 0; i < vec->n; i++)                                        \
      free(vec->v[i]);                                                         \
    free(vec->v);                                                              \
    vec->v = NULL;                                                             \
  }

// typedef struct {
//   size_t c, n, s;
//   void **v;
// } cvecp;
//
// [[nodiscard]] static inline cvecp cvecp_init(size_t s, size_t c) {
//   return (cvecp){.c = c, .n = 0, .s = s, .v = malloc(s * c)};
// }
//
// static inline size_t cvecp_expand_(cvecp *restrict vec) {
//   void *tmp = realloc(vec->v, vec->s * (vec->c << 1));
//   if (!tmp)
//     return 0;
//   vec->v = tmp;
//   vec->c <<= 1;
//   return vec->c;
// }
//
// static inline void cvecp_d(cvecp *restrict vec, const size_t i) {
//   if (i > vec->n)
//     return;
//   vec->n--;
//   if (i == vec->n)
//     return;
//   memmove(&((unsigned char *)vec->v)[i * vec->s],
//           &((unsigned char *)vec->v)[(i + 1) * vec->s], (vec->n - i) *
//           vec->s);
// }
//
// static inline void cvecp_destroy(cvecp *restrict vec) {
//   free(vec->v);
//   vec->v = NULL;
// }

#endif
