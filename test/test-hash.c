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

/* #define print_structest_debug(hz) */ \
  /* for (size_t i = 0; i < hz.n; i++) { */ \
    /* printf("%zu: %10d %pt", i, chm_kat(&hz, i), ((structest **)hz.vs)[i]); */ \
    /* if (chmt_vat(&hz, structest *, i) == TOMBSTONE) */ \
      /* printf("T"); */ \
    /* else if (((structest **)hz.vs)[i]) */ \
      /* printf("%zu: %10d {%d, %lu}", i, chm_kat(&hz, i), */ \
             /* ((structest **)hz.vs)[i]->x, ((structest **)hz.vs)[i]->y); */ \
    /* printf("tvat? %d", chm_vat(&hz, i)); */ \
    /* puts(""); */ \
  /* } */

#define print_structest_debug2(hz)                                             \
  for (size_t i = 0; i < (hz)->c; i++) {                                       \
    printf("%3zu: %10d (h:%2zu) %20p", i, *(uint32_t *)chm_kat(hz, i),         \
           hash_int(*(uint32_t *)chm_kat(hz, i)),                              \
           (structest **)chm_vat(hz, i));                                      \
    structest **vi = (structest **)chm_vat(hz, i);                             \
    if (*vi)                                                                   \
      printf(" -> {%d, %lu}", (*vi)->x, (*vi)->y);                             \
    puts("");                                                                  \
  }

// #define print_structest_debug2(hz) puts("todo")
#define print_int_debug(hx)                                                    \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10d (h:%2zu) %20u", i, *(uint32_t *)chm_kat(hx, i),         \
           (hx->khash)(chm_kat(hx, i)), *(uint32_t *)chm_vat(hx, i));          \
    printf("\t%p", (uint32_t *)chm_vat(hx, i));                                \
    /* if (((structest **)hx->vs)[i]) */                                       \
    /* printf(" -> {%d, %lu}", ((structest **)hx->vs)[i]->x, */                \
    /* ((structest **)hx->vs)[i]->y); */                                       \
    puts("");                                                                  \
  };

#define vlit(v)                                                                \
  &(typeof((v))) { (v) }
int main(int argc, char *argv[]) {
  // size_t s = 4;
  // void *xx = malloc(10 * s);
  //
  // printf("x = %p\n", xx);
  // printf("x+ = %p\n", ((unsigned char *)xx) + (2 * s));

  // chash hx = chash_init(sizeof(uint32_t), 10);
  // uint32_t _20 = 20;
  // uint32_t _21 = 21;
  // chash_i(&hx, 20, &_20);
  // chash_i(&hx, 20, &_21);
  // chash_i(&hx, 21, &(uint32_t){33});
  // chash_i(&hx, 23, vlit(uint32_t, 0xFFFFFFF));
  // // chasht_i(hx, 24, uint32_t, 20);
  // chash_d(&hx, 20);
  // // chasht_d(hx, 20, uint32_t);
  // //
  // //
  //
  // printf("*(uint32_t *)TOMBSTONE = %d\n", *(uint32_t *)TOMBSTONE);
  // for (size_t i = 0; i < hx.n; i++) {
  //   printf("%zu: %10d %10d\n", i, chm_kat(&hx, i), ((uint32_t *)hx.vs)[i]);
  // }
  // chash_i(&hx, 20, &_21);
  // for (size_t i = 0; i < hx.n; i++) {
  //   printf("%zu: %10d %10d\n", i, chm_kat(&hx, i), ((uint32_t *)hx.vs)[i]);
  // }
  // return EXIT_SUCCESS;

  chash *hz = chash_init(sizeof(uint32_t), sizeof(structest *));
  structest *t1 = malloc(sizeof *t1);
  t1->x = 20;
  t1->y = 20;
  structest *t2 = malloc(sizeof *t2);
  t2->x = 2;
  t2->y = 2;
  structest *t23 = &((structest){23, 23});
  structest *t25 = &((structest){25, 25});
  structest *t27 = &((structest){27, 27});
  structest *t29 = &((structest){29, 29});
  structest *t30 = &((structest){30, 30});
  structest *t31 = &((structest){31, 31});
  structest *t0 = &((structest){0, 0});

  uint32_t k23 = 23;
  printf("hz.s = %zu\n", hz->sv);
  chash_ikl(hz, 20, &t1);
  chash_i2(hz, &k23, &t23);
  chash_ikl(hz, 25, &t25);
  // chash_ikl(hz, 2, &t2);
  // chash_ikl(hz, 27, &t27);
  chash_ikl(hz, 29, &t29);
  chash_ikl(hz, 31, &t31);
  chash_ikl(hz, 0, &t0);

  print_structest_debug2(hz);

  // // NOTE: this is a weird and very specific ordering of the keys
  // // to force a circular insertion/deletion
  //
  // chash_i2(hz, 18, &t1);
  // // chasht_i(hz, 21, structest *, t2);
  // chash_i2(hz, 21, &t23);
  // chash_i2(hz, 2, &t2);
  // chash_i2(hz, 5, &t25);
  // chash_i2(hz, 8, &t27);
  // chash_i2(hz, 14, &t29);
  // // chash_i2(hz, 30, &t30);
  // chash_i2(hz, 17, &t31);
  // chash_i2(hz, 20, &t0);
  //
  // chash_d2(hz, 21);
  // chash_i2(hz, 23, &t23);

  print_structest_debug2(hz);

  // structest *getted = *(structest**)chash_g(hz, 1);
  // structest *gg = *getted;
  structest *getted = chash_gt(hz, 23, structest *);
  printf("getted = %p\n", getted);
  if (getted)
    printf("{%3d, %3lu}\n", getted->x, getted->y);
  else
    puts("NULL");
  puts("DELETE 23");
  chash_d2(hz, 23);
  print_structest_debug2(hz);

  // structest *hz_v20 = *(structest **)chash_g(&hz, 20);
  // printf("%d: {%d, %lu}\n", 20, hz_v20->x, hz_v20->y);
  //
  // structest *hz_v20_2 = {0};
  // memcpy(&hz_v20_2, chash_g(&hz, 20), hz.s);
  // printf("%d: {%d, %lu}\n", 20, hz_v20_2->x, hz_v20_2->y);
  //
  // structest *hz_v20t;
  // chasht_g(hz, 20, structest *, hz_v20t);
  // printf("%d: {%d, %lu}\n", 20, hz_v20t->x, hz_v20t->y);

  // chasht_d(hz, 21, structest *);
  puts("DELETE 25");
  chash_d2(hz, 25);
  print_structest_debug2(hz);
  puts("INSERT 31");
  chash_ikl(hz, 31, &t31);
  print_structest_debug2(hz);

  printf("h = %p\n", hz);

  size_t done = chash_resize2(hz, 1 << 4);
  printf("done = %zu\n", done);
  // printf("h->ks = %p\n", hz->ks);
  printf("h = %p\n", hz);
  print_structest_debug2(hz);

  done = chash_resize2(hz, 1 << 3);
  printf("done = %zu\n", done);
  print_structest_debug2(hz);
  // void *tomb = malloc(hz.s);
  // tomb = TOMBSTONE;
  // memcpy(tomb, &TOMBSTONE, hz.s);
  // printf("tomb == TOMBSTONE = %d\n", tomb == &TOMBSTONE);
  // printf("memcmp(tomb, &TOMBSTONE, hz.s) = %d\n", memcmp(tomb, &TOMBSTONE,
  // hz.s)); int check = *(structest **)((void *)&chm_vat(&hz, 0)) == TOMBSTONE;
  // printf("check = %d (%p)\n", check, &chm_vat(&hz, 0));
  // int checkt = ((structest **)(&hz)->vs)[(0)] == TOMBSTONE;
  // printf("checkt = %d (%p)\n", checkt, chmt_vat(&hz, structest *, 0));
  // printf("memcmp((void*)&chm_vat(&hz, 0), &TOMBSTONE, hz.s) = %d\n",
  // memcmp((void*)&chm_vat(&hz, 0), &TOMBSTONE, hz.s));
  // printf("(void*)&chm_vat(&hz, 0) == &TOMBSTONE = %d\n", (void*)&chm_vat(&hz,
  // 0) == &TOMBSTONE); printf("memcmp((void*)&chm_vat(&hz, 2), &TOMBSTONE,
  // hz.s) = %d\n", memcmp((void*)&chm_vat(&hz, 2), &TOMBSTONE, hz.s));
  // printf("(void*)&chm_vat(&hz, 2) == &TOMBSTONE = %d\n", (void*)&chm_vat(&hz,
  // 2) == &TOMBSTONE);

  // chash_i(&hz, 21, &t2);
  // chash_i(&hz, 21, &t2);
  // chash_i(&hz, 24, &t3);
  //
  // // chasht_i(hz, 21, structest *, t2);
  // print_structest_debug(hz);
  // chash_d(&hz, 21);
  // puts("");
  // print_structest_debug(hz);
  //
  // // structest *hz_v24t;
  // structest *hz_v24t = *(structest **)chash_g(&hz, 24);
  // // chasht_g(hz, 25, structest *, hz_v24t);
  // if (hz_v24t)
  //   printf("%d: {%d, %lu}\n", 24, hz_v24t->x, hz_v24t->y);
  // else
  //   printf("NULL\n");
  //
  // chasht_resize(hz, structest *, 20);
  // print_structest_debug(hz);

  free(t1);
  free(t2);
  chash_destroy(hz);

  puts("------------------------------------------------");
  puts("UINT32");

  chash *hx = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  chash_ikl(hx, 21, vlit(22));
  chash_ikl(hx, 25, vlit(25));
  chash_ikl(hx, 2, vlit(2));
  chash_ikl(hx, 27, vlit(27));
  chash_ikl(hx, 29, vlit(29));
  chash_ikl(hx, 31, vlit(31));
  // NOTE: this will be ignored. Both key and value of 0 is not permitted.
  chash_ikl(hx, 0, vlit(0));
  chash_ikl(hx, 3, vlit(3));
  print_int_debug(hx);

  uint32_t *igetted = (uint32_t *)chash_g(hz, 25);
  printf("igetted = %p -> ", igetted);
  if (igetted)
    printf("%u\n", *igetted);
  else
    puts("NULL");
  igetted = (uint32_t *)chash_g(hz, 26);
  printf("igetted = %p -> ", igetted);
  if (igetted)
    printf("%u\n", *igetted);
  else
    puts("NULL");
  puts("DELETE 23");
  chash_d2(hz, 25);

  print_int_debug(hx);
  puts("INSERT 31");
  chash_ikl(hz, 31, vlit(32));
  print_int_debug(hx);

  printf("h = %p\n", hz);

  done = chash_resize2(hx, 1 << 4);
  printf("done = %zu\n", done);
  print_int_debug(hx);

  done = chash_resize2(hx, 1 << 3);
  printf("done = %zu\n", done);
  print_int_debug(hx);
  return EXIT_SUCCESS;
}
