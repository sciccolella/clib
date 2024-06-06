#include "../cvect.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t x;
  uint64_t y;
} structest;

int main(int argc, char *argv[]) {
  puts("[cvect] uint32_t");
  cvec x = cvect_init(sizeof(uint32_t), 2);

  cvect_a(x, uint32_t, 19);
  cvect_a(x, uint32_t, 19);
  cvect_a(x, uint32_t, 18);
  cvect_a(x, uint32_t, 17);
  cvect_a(x, uint32_t, 16);
  cvect_a(x, uint32_t, 15);
  cvect_a(x, uint32_t, 14);
  cvect_a(x, uint32_t, 13);
  cvect_a(x, uint32_t, 12);
  cvect_a(x, uint32_t, 11);
  cvect_a(x, uint32_t, 10);
  cvect_i(x, uint32_t, 21, 3);
  cvect_a(x, uint32_t, 66);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", ((uint32_t *)x.v)[i]);
  }
  puts("");

  cvect_i(x, uint32_t, 44, 7);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", cvect_g(x, uint32_t, i));
  }
  puts("");

  cvect_d(&x, 2);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", cvect_g(x, uint32_t, i));
  }
  puts("");

  cvect_destroy(&x);

  puts("[cvect] uint64_t");
  cvec y = cvect_init(sizeof(uint64_t), 2);
  cvect_a(y, uint64_t, 19);
  cvect_a(y, uint64_t, 19);
  cvect_a(y, uint64_t, 18);
  cvect_a(y, uint64_t, 17);
  cvect_a(y, uint64_t, 16);
  cvect_a(y, uint64_t, 15);
  cvect_a(y, uint64_t, 14);
  cvect_a(y, uint64_t, 13);
  cvect_a(y, uint64_t, 12);
  cvect_a(y, uint64_t, 11);
  cvect_a(y, uint64_t, 10);
  cvect_i(y, uint64_t, 21, 3);
  cvect_a(y, uint64_t, 66);
  for (size_t i = 0; i < y.n; i++) {
    printf("%lu ", ((uint64_t *)y.v)[i]);
  }
  puts("");

  cvect_i(y, uint64_t, 44, 7);
  for (size_t i = 0; i < y.n; i++) {
    printf("%lu ", cvect_g(y, uint64_t, i));
  }
  puts("");

  cvect_d(&y, 2);
  for (size_t i = 0; i < y.n; i++) {
    printf("%lu ", cvect_g(y, uint64_t, i));
  }
  puts("");
  cvect_destroy(&y);

  puts("[cvect] structest *");
  cvec z = cvect_init(sizeof(structest *), 2);
  structest *t1 = malloc(sizeof *t1);
  t1->x = 21;
  t1->y = 22;
  cvect_a(z, structest *, t1);
  structest *t2 = malloc(sizeof *t2);
  t2->x = 11;
  t2->y = 12;
  cvect_a(z, structest *, t2);
  structest *t3 = malloc(sizeof *t3);
  t3->x = 1;
  t3->y = 2;
  cvect_a(z, structest *, t3);
  cvect_i(z, structest *, t3, 1);

  for (size_t i = 0; i < z.n; i++) {
    printf("%zu: {%d, %lu}\n", i, ((structest **)z.v)[i]->x,
           ((structest **)z.v)[i]->y);
  }

  for (size_t i = 0; i < z.n; i++) {
    structest *tmp = cvect_g(z, structest *, i);
    printf("%zu: {%d, %lu}\n", i, tmp->x, tmp->y);
  }

  free(t1);
  free(t2);
  free(t3);
  cvect_destroy(&z);

  return EXIT_SUCCESS;
}
