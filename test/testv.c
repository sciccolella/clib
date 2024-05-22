#include "../cvecn.h"
#include "../chash.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t x;
  uint64_t y;
} structest;

// CVEC_DECLAREP(structest *, structestp);

#define vlit(type, value) &(type){(value)}
int main(int argc, char *argv[]) {
  size_t s = 4;
  void *xx = malloc(10 * s);

  printf("x = %p\n", xx);
  printf("x+ = %p\n", ((unsigned char *)xx) + (2 * s));

  cvec x = cvec_init(sizeof(uint32_t), 2);
  cvec_a(&x, uint32_t, 19);
  cvec_a(&x, uint32_t, 18);
  cvec_a(&x, uint32_t, 17);
  cvec_a(&x, uint32_t, 16);
  cvec_a(&x, uint32_t, 15);
  cvec_a(&x, uint32_t, 14);
  cvec_a(&x, uint32_t, 13);
  cvec_a(&x, uint32_t, 12);
  cvec_a(&x, uint32_t, 11);
  cvec_a(&x, uint32_t, 10);
  cvec_a(&x, uint32_t, 9);
  cvec_a(&x, uint32_t, 8);
  cvec_a(&x, uint32_t, 7);
  cvec_a(&x, uint32_t, 6);

  cvec_i(&x, uint32_t, 21, 3);
  cvec_a(&x, uint32_t, 66);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", ((uint32_t *)x.v)[i]);
  }
  puts("\n");
  cvec_i(&x, uint32_t, 44, 7);
  cvec_d(&x, 2);
  for (size_t i = 0; i < x.n; i++) {
    printf("%d ", ((uint32_t *)x.v)[i]);
  }
  puts("\n");

  chash hx = chash_init(sizeof(uint32_t), 10);
  uint32_t _20 = 20;
  uint32_t _21 = 21;
  chash_i(&hx, 20, &_20);
  chash_i(&hx, 20, &_21);
  chash_i(&hx, 21, &(uint32_t){33});
  chash_i(&hx, 23, vlit(uint32_t, 12));
  // chasht_i(hx, 20, uint32_t, 20);

  for (size_t i = 0; i < hx.n; i++) {
    printf("%zu: %5d=%5d\n", i, hx.ks[i], chash_vat(&hx, i));
  }



  return EXIT_SUCCESS;
  cvec_destroy(&x);

  cvec y = cvec_init(sizeof(uint64_t), 2);
  cvec_a(&y, uint64_t, 19);
  cvec_a(&y, uint64_t, 18);
  cvec_a(&y, uint64_t, 17);
  cvec_a(&y, uint64_t, 16);
  cvec_a(&y, uint64_t, 15);
  cvec_a(&y, uint64_t, 14);
  cvec_a(&y, uint64_t, 13);
  cvec_a(&y, uint64_t, 12);
  cvec_a(&y, uint64_t, 11);
  cvec_a(&y, uint64_t, 10);
  cvec_a(&y, uint64_t, 9);
  cvec_a(&y, uint64_t, 8);
  cvec_a(&y, uint64_t, 7);
  cvec_a(&y, uint64_t, 6);

  cvec_i(&y, uint64_t, 21, 3);
  cvec_a(&y, uint64_t, 5);
  cvec_i(&y, uint64_t, 31, 3);
  for (size_t i = 0; i < y.n; i++) {
    printf("%lu ", ((uint64_t *)y.v)[i]);
  }
  puts("\n");
  cvec_destroy(&y);

  cvec z = cvec_init(sizeof(structest *), 2);
  structest *t1 = malloc(sizeof *t1);
  t1->x = 21;
  t1->y = 22;
  cvec_a(&z, structest *, t1);
  structest *t2 = malloc(sizeof *t2);
  t2->x = 11;
  t2->y = 12;
  cvec_a(&z, structest *, t2);
  structest *t3 = malloc(sizeof *t3);
  t3->x = 1;
  t3->y = 2;
  cvec_a(&z, structest *, t3);
  cvec_i(&z, structest *, t3, 1);


  for (size_t i = 0; i < z.n; i++) {
    printf("%zu: {%d, %lu}\n", i, ((structest **) z.v)[i]->x, ((structest **) z.v)[i]->y);
  }
  free(xx);
  return EXIT_SUCCESS;
}
