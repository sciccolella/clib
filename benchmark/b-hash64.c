#include "../chash.h"
#include "khash.h"
#include "tracing.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>

#define vlit(v)                                                                \
  &(__typeof__((v))) { (v) }

#define vlit64(v)                                                              \
  &(uint64_t) { (v) }

KHASH_MAP_INIT_INT(64, uint64_t)
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
  khash_t(64) *h = kh_init(64);

  trace_t s;
  size_t MAX = 5000000;

  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    k = kh_put(64, h, key, &ret);
    kh_value(h, k) = value;
  }
  printtrace_diffnow(&s, "khash ");

  s = trace_start();
  chash *ch = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_il(ch, vlit(key), vlit(value));
  }
  printtrace_diffnow(&s, "clib-l");

#ifdef __AVX2__
  s = trace_start();
  chash *ch2 = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_i_avx2_u8(ch2, vlit64(key), vlit64(value));
  }
  printtrace_diffnow(&s, "clib-s");
#endif

#ifdef __AVX512F__
  s = trace_start();
  chash *ch512 = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_i_avx512f_u4(ch512, vlit64(key), vlit64(value));
  }
  printtrace_diffnow(&s, "clib-f");
#endif

  s = trace_start();
  chash *chAUTO = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_i(chAUTO, vlit64(key), vlit64(value));
  }
  printtrace_diffnow(&s, "clib-A");
  return EXIT_SUCCESS;
}
