#include "../cvec.h"
// #include "../cvecn.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t x;
  uint64_t y;
} structest;

CVEC_DECLAREP(structest *, structestp);

#undef cvec_a
#define cvec_a(v, x)                                                           \
  _Generic(vec0(v), structest *: cvec_structestp_a, cvec_S_a)(v, x)

#undef cvec_i
#define cvec_i(v, x, i)                                                        \
  _Generic(vec0(v), structest *: cvec_structestp_i, cvec_S_i)(v, x, i)

#undef cvec_d
#define cvec_d(v, i)                                                           \
  _Generic(vec0(v), structest *: cvec_structestp_d, cvec_S_d)(v, i)

#undef cvec_destroy
#define cvec_destroy(v)                                                        \
  _Generic(vec0(v), structest *: cvec_structestp_destroy, cvec_S_destroy)(v)

#undef cvec_init
#define cvec_init(type, capacity)                                              \
  _Generic(CVEC_TYPE_INIT(type),                                               \
      structest *: cvec_structestp_init,                                       \
      cvec_S_init)(capacity)


int main(int argc, char *argv[]) {
  cvec_uint32_t x = cvec_init(uint32_t, 2);
  cvec_a(&x, 19);
  cvec_a(&x, 18);
  cvec_a(&x, 17);
  cvec_a(&x, 16);
  cvec_a(&x, 15);
  cvec_a(&x, 14);
  cvec_a(&x, 13);
  cvec_a(&x, 12);
  cvec_a(&x, 11);
  cvec_a(&x, 10);
  cvec_a(&x, 9);
  cvec_a(&x, 8);
  cvec_a(&x, 7);
  cvec_a(&x, 6);

  cvec_i(&x, 21, 3);
  cvec_a(&x, 66);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", ((uint32_t *)x.v)[i]);
  }
  puts("\n");
  cvec_d(&x, 2);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", ((uint32_t *)x.v)[i]);
  }
  puts("\n");
  // // cvec_d(&x, 0);
  cvec_destroy(&x);

  // cvec_uint64_t y = cvec_uint64_t_init(2);
  cvec_uint64_t y = cvec_init(uint64_t, 2);
  cvec_a(&y, 19);
  cvec_a(&y, 18);
  cvec_a(&y, 17);
  cvec_a(&y, 16);
  cvec_a(&y, 15);
  cvec_a(&y, 14);
  cvec_a(&y, 13);
  cvec_a(&y, 12);
  cvec_a(&y, 11);
  cvec_a(&y, 10);
  cvec_a(&y, 9);
  cvec_a(&y, 8);
  cvec_a(&y, 7);
  cvec_a(&y, 6);

  cvec_i(&y, 21, 3);

  cvec_a(&y, 5);
  cvec_i(&y, 31, 3);
  for (size_t i = 0; i < y.n; i++) {
    printf("%lu ", y.v[i]);
  }
  puts("\n");
  cvec_destroy(&y);

  structest t0 = {1, 2};
  // cvec_structestp z = cvec_structestp_init(2);
  cvec_structestp z = cvec_init(structest *, 2);
  cvec_structestp_a(&z, &t0);
  cvec_structestp_a(&z, &(structest){3, 4});
  cvec_structestp_a(&z, &(structest){5, 6});
  cvec_structestp_a(&z, &(structest){7, 8});
  cvec_structestp_a(&z, &(structest){9, 10});
  cvec_structestp_i(&z, &(structest){11, 12}, 2);
  cvec_structestp_d(&z, 1);

  for (size_t i = 0; i < z.n; i++) {
    printf("%zu: {%d, %lu}\n", i, z.v[i]->x, z.v[i]->y);
  }
  cvec_structestp_destroy(&z);

  // cvec_structestp z2 = cvec_structestp_init(2);
  cvec_structestp z2 = cvec_init(structest *, 2);
  structest *t1 = malloc(sizeof *t1);
  t1->x = 21;
  t1->y = 22;
  cvec_structestp_a(&z2, t1);
  structest *t2 = malloc(sizeof *t1);
  t1->x = 21;
  t1->y = 22;
  cvec_structestp_a(&z2, t2);
  structest *t3 = malloc(sizeof *t1);
  t1->x = 21;
  t1->y = 22;
  cvec_structestp_a(&z2, t3);
  cvec_structestp_destroy_iter(&z2);

  return EXIT_SUCCESS;
}
