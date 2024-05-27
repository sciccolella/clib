#include "../chash.h"
#include "../cvecn.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t x;
  uint64_t y;
} structest;

// CVEC_DECLAREP(structest *, structestp);

#define print_structest_debug(hz)                                              \
  for (size_t i = 0; i < hz.n; i++) {                                          \
    printf("%zu: %10d %p\t", i, chm_kat(&hz, i), ((structest **)hz.vs)[i]);    \
    if (chmt_vat(&hz, structest *, i) == TOMBSTONE)                            \
      printf("T");                                                             \
    else if (((structest **)hz.vs)[i])                                         \
      printf("%zu: %10d {%d, %lu}", i, chm_kat(&hz, i),                        \
             ((structest **)hz.vs)[i]->x, ((structest **)hz.vs)[i]->y);        \
    puts("");                                                                  \
  }

#define vlit(type, value)                                                      \
  &(type) { (value) }
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
  chash_i(&hx, 23, vlit(uint32_t, 0xFFFFFFF));
  // chasht_i(hx, 20, uint32_t, 20);

  for (size_t i = 0; i < hx.n; i++) {
    printf("%zu: %10d %10d\n", i, chm_kat(&hx, i), ((uint32_t *)hx.vs)[i]);
  }

  chash hz = chash_init(sizeof(structest *), 10);
  structest *t1 = malloc(sizeof *t1);
  t1->x = 21;
  t1->y = 22;
  structest *t2 = malloc(sizeof *t2);
  t2->x = 11;
  t2->y = 12;
  structest *t3 = malloc(sizeof *t3);
  t3->x = 1;
  t3->y = 2;

  printf("hz.s = %zu\n", hz.s);
  chash_i(&hz, 20, &t1);
  chasht_i(hz, 21, structest *, t2);
  chash_i(&hz, 23, &t1);

  print_structest_debug(hz);

  structest *hz_v20 = *(structest **)chash_g(&hz, 20);
  printf("%d: {%d, %lu}\n", 20, hz_v20->x, hz_v20->y);

  structest *hz_v20_2 = {0};
  memcpy(&hz_v20_2, chash_g(&hz, 20), hz.s);
  printf("%d: {%d, %lu}\n", 20, hz_v20_2->x, hz_v20_2->y);

  structest *hz_v20t;
  chasht_g(hz, 20, structest *, hz_v20t);
  printf("%d: {%d, %lu}\n", 20, hz_v20t->x, hz_v20t->y);

  chasht_d(hz, 21, structest *);
  print_structest_debug(hz);
  chash_i(&hz, 21, &t2);
  print_structest_debug(hz);



  return EXIT_SUCCESS;
}
