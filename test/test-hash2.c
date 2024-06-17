#define CHDEBUG
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

#define print_int_debug(hx)                                                    \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10d (h:%2zu) %20u", i, *(uint32_t *)chm_kat(hx, i),         \
           (hx->khash)(chm_kat(hx, i)), *(uint32_t *)chm_vat(hx, i));          \
    printf("\t%p", (uint32_t *)chm_vat(hx, i));                                \
    puts("");                                                                  \
  };

#define vlit(v)                                                                \
  &(__typeof__((v))) { (v) }
int main(int argc, char *argv[]) {

#ifdef __AVX2__
  puts("avx2");
#endif
  puts("------------------------------------------------");
  puts("UINT32");

  chash *hx = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  chash *hs = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  printf("hs->c = %zu\n", hs->c);
  chash_i(hx, vlit(21), vlit(21));
  chash_i_su4_avx512f(hs, vlit(21), vlit(21));
  chash_i(hx, vlit(25), vlit(25));
  chash_i_su4_avx512f(hs, vlit(25), vlit(25));
  chash_i(hx, vlit(2), vlit(2));
  chash_i_su4_avx512f(hs, vlit(2), vlit(2));
  chash_i(hx, vlit(27), vlit(28));
  chash_i_su4_avx512f(hs, vlit(27), vlit(28));
  //
  chash_i(hx, vlit(27), vlit(57));
  chash_i_su4_avx512f(hs, vlit(27), vlit(57));
  // chash_ikl(hx, 29, vlit(29));
  // chash_ikl(hx, 31, vlit(31));
  // // NOTE: this will be ignored. Both key and value of 0 is not permitted.
  // chash_ikl(hx, 0, vlit(0));
  // chash_ikl(hx, 3, vlit(3));
  // print_int_debug(hx);

  // uint32_t *igetted = (uint32_t *)chash_g(hx, vlit(25));
  // printf("igetted (25) = %p -> ", igetted);
  // if (igetted)
  //   printf("%u\n", *igetted);
  // else
  //   puts("NULL");
  // igetted = (uint32_t *)chash_g(hx, vlit(26));
  // printf("igetted (26) = %p -> ", igetted);
  // if (igetted)
  //   printf("%u\n", *igetted);
  // else
  //   puts("NULL");
  // puts("DELETE 25");
  // chash_d(hx, vlit(25));
  //
  // print_int_debug(hx);
  // puts("INSERT 31");
  // chash_ikl(hx, 31, vlit(32));
  // print_int_debug(hx);
  //
  // printf("h = %p\n", hx);
  //
  // size_t done2 = chash_resize(hx, 1 << 4);
  // printf("done = %zu\n", done2);
  // print_int_debug(hx);
  //
  // done2 = chash_resize(hx, 1 << 3);
  // printf("done = %zu\n", done2);
  // print_int_debug(hx);
  return EXIT_SUCCESS;
}
