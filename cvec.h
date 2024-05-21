#ifndef CVEC_H
#define CVEC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CVEC_DECLARE(type, typename)                                           \
  typedef struct {                                                             \
    size_t c, n;                                                               \
    type *v;                                                                   \
  } cvec_##typename;                                                           \
                                                                               \
  static inline cvec_##typename cvec_##typename##_init(size_t capacity) {      \
    return (cvec_##typename){                                                  \
        .c = capacity, .n = 0, .v = malloc(sizeof(type) * capacity)};          \
  }                                                                            \
                                                                               \
  static inline void cvec_##                                                   \
      typename##_destroy(cvec_##typename *restrict vec) {                      \
    free(vec->v);                                                              \
    vec->v = NULL;                                                             \
  }                                                                            \
                                                                               \
  static inline size_t cvec_##                                                 \
      typename##_expand(cvec_##typename *restrict vec) {                       \
    type *tmp = realloc(vec->v, sizeof *vec->v * (vec->c << 1));               \
    if (!tmp)                                                                  \
      return 0;                                                                \
    vec->v = tmp;                                                              \
    vec->c <<= 1;                                                              \
    return vec->c;                                                             \
  }                                                                            \
                                                                               \
  static inline type cvec_##                                                   \
      typename##_a(cvec_##typename *restrict vec, type x) {                    \
    if (vec->n >= vec->c)                                                      \
      if (!cvec_##typename##_expand(vec))                                      \
        return (type)0;                                                        \
                                                                               \
    vec->v[vec->n] = x;                                                        \
    vec->n++;                                                                  \
    return x;                                                                  \
  }                                                                            \
                                                                               \
  static inline type cvec_##                                                   \
      typename##_i(cvec_##typename *restrict vec, type x, const size_t i) {    \
    if (i > vec->c)                                                            \
      return cvec_##typename##_a(vec, x);                                      \
    if (vec->n >= vec->c)                                                      \
      if (!cvec_##typename##_expand(vec))                                      \
        return (type)0;                                                        \
                                                                               \
    memmove(&vec->v[i + 1], &vec->v[i], (vec->n - i) * sizeof(type));          \
    vec->v[i] = x;                                                             \
                                                                               \
    return x;                                                                  \
  }                                                                            \
                                                                               \
  static inline type cvec_##                                                   \
      typename##_d(cvec_##typename *restrict vec, const size_t i) {            \
    if (i > vec->n)                                                            \
      return (type)0;                                                          \
    vec->n--;                                                                  \
    if (i == vec->n)                                                           \
      return vec->v[vec->n + 1];                                               \
    type ret = vec->v[i];                                                      \
    memmove(&vec->v[i], &vec->v[i + 1], (vec->n - i) * sizeof(type));          \
    return ret;                                                                \
  }

#define CVEC_DECLAREP(type, typename)                                          \
  CVEC_DECLARE(type, typename)                                                 \
  static inline void cvec_##                                                   \
      typename##_destroy_iter(cvec_##typename *restrict vec) {                 \
    for (size_t i = 0; i < vec->n; i++)                                        \
      free(vec->v[i]);                                                         \
    free(vec->v);                                                              \
    vec->v = NULL;                                                             \
  }

/* #define CVEC_DECLAREP(type, typename) \
  typedef struct { \
    size_t c, n; \
    type *v; \
  } cvec_##typename; \
                                                                               \ \
  static inline cvec_##typename cvec_##typename##_init(size_t capacity) { \
    return (cvec_##typename){ \
        .c = capacity, .n = 0, .v = malloc(sizeof(type) * capacity)}; \
  } \
                                                                               \ \
  static inline void cvec_## \
      typename##_destroy(cvec_##typename *restrict vec) { \
    free(vec->v); \
    vec->v = NULL; \
  } \
                                                                               \ \
  static inline void cvec_## \
      typename##_destroy_iter(cvec_##typename *restrict vec) { \
    for (size_t i = 0; i < vec->n; i++) \
      free(vec->v[i]); \
    free(vec->v); \
    vec->v = NULL; \
  } \
                                                                               \ \
  static inline size_t cvec_## \
      typename##_expand(cvec_##typename *restrict vec) { \
    type *tmp = realloc(vec->v, sizeof *vec->v * (vec->c << 1)); \
    if (!tmp) \
      return 0; \
    vec->v = tmp; \
    vec->c <<= 1; \
    return vec->c; \
  } \
                                                                               \ \
  static inline type cvec_## \
      typename##_a(cvec_##typename *restrict vec, type x) { \
    if (vec->n >= vec->c) \
      if (!cvec_##typename##_expand(vec)) \
        return (type)NULL; \
                                                                               \ \
    vec->v[vec->n] = x; \
    vec->n++; \
    return x; \
  } \
                                                                               \ \
  static inline type cvec_## \
      typename##_i(cvec_##typename *restrict vec, type x, const size_t i) { \
    if (i > vec->c) \
      return cvec_##typename##_a(vec, x); \
    if (vec->n >= vec->c) \
      if (!cvec_##typename##_expand(vec)) \
        return (type)NULL; \
                                                                               \ \
    memmove(&vec->v[i + 1], &vec->v[i], (vec->n - i) * sizeof(type)); \
    vec->v[i] = x; \
                                                                               \ \
    return x; \
  } \
                                                                               \ \
  static inline type cvec_## \
      typename##_d(cvec_##typename *restrict vec, const size_t i) { \
    if (i > vec->n) \
      return (type)NULL; \
    if (i == vec->n) { \
      vec->n--; \
      return vec->v[vec->n + 1]; \
    } \
    type ret = vec->v[i]; \
    memmove(&vec->v[i], &vec->v[i + 1], (vec->n - i - 1) * sizeof(type)); \
                                                                               \ \
    return ret; \
  } */

CVEC_DECLARE(uint8_t, uint8_t);
CVEC_DECLARE(uint16_t, uint16_t);
CVEC_DECLARE(uint32_t, uint32_t);
CVEC_DECLARE(uint64_t, uint64_t);
CVEC_DECLARE(int8_t, int8_t);
CVEC_DECLARE(int16_t, int16_t);
CVEC_DECLARE(int32_t, int32_t);
CVEC_DECLARE(int64_t, int64_t);
CVEC_DECLARE(char, char);
CVEC_DECLARE(float, float);
CVEC_DECLARE(double, double);
CVEC_DECLARE(long double, long_double);

#define vec0(v_) ((v_)->v[0])

#define cvec_S_a                                                               \
  uint8_t : cvec_uint8_t_a,                                                    \
            uint16_t : cvec_uint16_t_a,                                        \
                       uint32_t : cvec_uint32_t_a,                             \
                                  uint64_t : cvec_uint64_t_a,                  \
                                             int8_t : cvec_int8_t_a,           \
                                                      int16_t                  \
      : cvec_int16_t_a,                                                        \
        int32_t : cvec_int32_t_a,                                              \
                  int64_t : cvec_int64_t_a,                                    \
                            char : cvec_char_a,                                \
                                   float : cvec_float_a,                       \
                                           double : cvec_double_a,             \
                                                    long double                \
      : cvec_long_double_a
#define cvec_a(v, x) _Generic(vec0(v), cvec_S_a)(v, x)

#define cvec_S_i                                                               \
  uint8_t : cvec_uint8_t_i,                                                    \
            uint16_t : cvec_uint16_t_i,                                        \
                       uint32_t : cvec_uint32_t_i,                             \
                                  uint64_t : cvec_uint64_t_i,                  \
                                             int8_t : cvec_int8_t_i,           \
                                                      int16_t                  \
      : cvec_int16_t_i,                                                        \
        int32_t : cvec_int32_t_i,                                              \
                  int64_t : cvec_int64_t_i,                                    \
                            char : cvec_char_i,                                \
                                   float : cvec_float_i,                       \
                                           double : cvec_double_i,             \
                                                    long double                \
      : cvec_long_double_i
#define cvec_i(v, x, i) _Generic(vec0(v), cvec_S_i)(v, x, i)

#define cvec_S_d                                                               \
  uint8_t : cvec_uint8_t_d,                                                    \
            uint16_t : cvec_uint16_t_d,                                        \
                       uint32_t : cvec_uint32_t_d,                             \
                                  uint64_t : cvec_uint64_t_d,                  \
                                             int8_t : cvec_int8_t_d,           \
                                                      int16_t                  \
      : cvec_int16_t_d,                                                        \
        int32_t : cvec_int32_t_d,                                              \
                  int64_t : cvec_int64_t_d,                                    \
                            char : cvec_char_d,                                \
                                   float : cvec_float_d,                       \
                                           double : cvec_double_d,             \
                                                    long double                \
      : cvec_long_double_d
#define cvec_d(v, i) _Generic(vec0(v), cvec_S_d)(v, i)

#define cvec_S_destroy                                                         \
  uint8_t : cvec_uint8_t_destroy,                                              \
            uint16_t : cvec_uint16_t_destroy,                                  \
                       uint32_t : cvec_uint32_t_destroy,                       \
                                  uint64_t : cvec_uint64_t_destroy,            \
                                             int8_t : cvec_int8_t_destroy,     \
                                                      int16_t                  \
      : cvec_int16_t_destroy,                                                  \
        int32_t : cvec_int32_t_destroy,                                        \
                  int64_t : cvec_int64_t_destroy,                              \
                            char : cvec_char_destroy,                          \
                                   float : cvec_float_destroy,                 \
                                           double : cvec_double_destroy,       \
                                                    long double                \
      : cvec_long_double_destroy
#define cvec_destroy(v) _Generic(vec0(v), cvec_S_destroy)(v)

#define cvec_S_init                                                            \
  uint8_t : cvec_uint8_t_init,                                                 \
            uint16_t : cvec_uint16_t_init,                                     \
                       uint32_t : cvec_uint32_t_init,                          \
                                  uint64_t : cvec_uint64_t_init,               \
                                             int8_t : cvec_int8_t_init,        \
                                                      int16_t                  \
      : cvec_int16_t_init,                                                     \
        int32_t : cvec_int32_t_init,                                           \
                  int64_t : cvec_int64_t_init,                                 \
                            char : cvec_char_init,                             \
                                   float : cvec_float_init,                    \
                                           double : cvec_double_init,          \
                                                    long double                \
      : cvec_long_double_init


#define CVEC_TYPE_INIT(type) (type)0
#define cvec_init(type, capacity)                                              \
  _Generic(CVEC_TYPE_INIT(type), cvec_S_init)(capacity)
#endif
