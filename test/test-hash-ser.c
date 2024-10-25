#undef __x86_64__
#include "../chash.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define print_int64_debug(hx)                                                  \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10ld (h:%2zu) %20lu", i, *(uint64_t *)chm_kat(hx, i),       \
           (hx->khash)(chm_kat(hx, i)) & hx->mod,                              \
           *(uint64_t *)chm_vat(hx, i));                                       \
    printf("\t%p", (uint64_t *)chm_vat(hx, i));                                \
    /* if (((structest **)hx->vs)[i]) */                                       \
    /* printf(" -> {%d, %lu}", ((structest **)hx->vs)[i]->x, */                \
    /* ((structest **)hx->vs)[i]->y); */                                       \
    puts("");                                                                  \
  };

#define vlit64(v)                                                              \
  &(uint64_t) { (v) }

int main(int argc, char *argv[]) {
  puts("------------------------------------------------");
  puts("UINT64");

  chash *h64 = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  chash_i(h64, vlit64(21), vlit64(21));
  chash_i(h64, vlit64(25), vlit64(25));
  chash_i(h64, vlit64(2), vlit64(2));
  chash_i(h64, vlit64(27), vlit64(27));
  // NOTE: this will be ignored. Both key and value of 0 is not permitted.
  chash_i(h64, vlit64(0), vlit64(0));
  chash_i(h64, vlit64(3), vlit64(3));
  print_int64_debug(h64);

  uint64_t *got64;
  got64 = (uint64_t *)chash_g(h64, vlit64(25));
  printf("got (25) = %p -> ", got64);
  if (got64)
    printf("%lu\n", *got64);
  else
    puts("NULL");
  got64 = (uint64_t *)chash_g(h64, vlit64(26));
  printf("got (26) = %p -> ", got64);
  if (got64)
    printf("%lu\n", *got64);
  else
    puts("NULL");
  puts("DELETE 25");
  chash_d(h64, vlit64(25));
  puts("REPLACE VAL(3): 3->25");
  chash_i(h64, vlit64(3), vlit64(25));
  print_int64_debug(h64);

  FILE *fout = fopen("cht.bin", "wb");
  if (!chash_sda(h64, fout))
    return EXIT_FAILURE;
  fclose(fout);

  FILE *fin = fopen("cht.bin", "rb");
  chash *lh64 = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  size_t _howmanyread = chash_dda(lh64, fin);
  printf("_howmanyread = %d\n", _howmanyread);
  fclose(fin);
  print_int64_debug(lh64);

  return EXIT_SUCCESS;
}
