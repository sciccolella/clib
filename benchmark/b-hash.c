#include "../chash.h"
#include "khash.h"
#include "tracing.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define print_int_debug(hx)                                                    \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10d (h:%2zu) %20u", i, *(uint32_t *)chm_kat(hx, i),         \
           (hx->khash)(chm_kat(hx, i))&hx->mod, *(uint32_t *)chm_vat(hx, i));          \
    printf("\t%p", (uint32_t *)chm_vat(hx, i));                                \
    puts("");                                                                  \
  };

#define vlit(v)                                                                \
  &(__typeof__((v))) { (v) }

// #define __AVX2__
KHASH_MAP_INIT_INT(32, uint32_t)
int main() {
#ifdef __AVX2__
  puts("[INFO] avx2 enabled");
#endif
#ifdef __AVX512F__
  puts("[INFO] avx-512f enabled");
#endif
  int ret;
  uint32_t key, value;
  khiter_t k;
  khash_t(32) *h = kh_init(32);
  uint32_t *get;

  trace_t s;
  size_t MAX = 5000000;
  // MAX = 10000;
  int SEED = 21;

  // -----------------------------------------
  //  KHASH

  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    k = kh_put(32, h, key, &ret);
    if (k != kh_end(h)) {
      kh_value(h, k) = key;
    }
  }
  printtrace_diffnow(&s, "khash  I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    k = kh_get(32, h, key);
    assert(kh_value(h, k) == key);
  }
  printtrace_diffnow(&s, "khash  G");

  // -----------------------------------------
  //  CHASH LINEAR

  srand(SEED);
  s = trace_start();
  chash *ch = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    chash_il(ch, vlit(key), vlit(key));
  }
  printtrace_diffnow(&s, "clib-l I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    get = (uint32_t *)chash_gl(ch, vlit(key));
    assert(*get == key);
  }
  printtrace_diffnow(&s, "clib-l G");

  // -----------------------------------------
  //  CHASH AVX2
#ifdef __AVX2__
  srand(SEED);
  s = trace_start();
  chash *ch2 = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    chash_i_avx2_u4(ch2, vlit(key), vlit(key));
  }
  printtrace_diffnow(&s, "clib-s I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    get = (uint32_t *)chash_g_avx2_u4(ch2, vlit(key));
    assert(*get == key);
  }
  printtrace_diffnow(&s, "clib-s G");
#endif

  // -----------------------------------------
  //  CHASH AVX512f
#ifdef __AVX512F__
  srand(SEED);
  s = trace_start();
  chash *ch512 = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_i_avx512f_u4(ch512, vlit(key), vlit(value));
  }
  printtrace_diffnow(&s, "clib-f I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    get = (uint32_t *)chash_g_avx512f_u4(ch512, vlit(key));
    // assert(*get == value);
  }
  printtrace_diffnow(&s, "clib-f G");
#endif

  // -----------------------------------------
  //  CHASH AUTO
  srand(SEED);
  s = trace_start();
  chash *chAUTO = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    chash_i(chAUTO, vlit(key), vlit(key));
  }
  printtrace_diffnow(&s, "clib-x I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    get = (uint32_t *)chash_g(chAUTO, vlit(key));
    assert(*get == key);
  }
  printtrace_diffnow(&s, "clib-x G");
  // -----------------------------------------
  //  CHASH AUTO 2
  srand(SEED);
  s = trace_start();
  chash *chAUTO2 = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    chash_iA(chAUTO2, vlit(key), vlit(key));
  }
  printtrace_diffnow(&s, "clib-A I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    get = (uint32_t *)chash_gA(chAUTO2, vlit(key));
    assert(*get == key);
  }
  printtrace_diffnow(&s, "clib-A G");
  // -----------------------------------------
  //  CHASH POINTER
  srand(SEED);
  s = trace_start();
  chash *chp = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    chash_pi(chp, vlit(key), vlit(key));
  }
  printtrace_diffnow(&s, "clib-P I");

  // get
  srand(SEED);
  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand();
    get = (uint32_t *)chash_pg(chp, vlit(key));
    assert(*get == key);
  }
  printtrace_diffnow(&s, "clib-P G");
  return EXIT_SUCCESS;
}
