#include "../chash.h"
#include "khash.h"
#include "tracing.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>

#define vlit(v)                                                                \
  &(__typeof__((v))) { (v) }

KHASH_MAP_INIT_INT(32, uint32_t)
int main() {
  int ret;
  uint32_t key, value;
  khiter_t k;
  khash_t(32) *h = kh_init(32);

  trace_t s;
  size_t MAX = 5000000;

  s = trace_start();
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    k = kh_put(32, h, key, &ret);
    kh_value(h, k) = value;
  }
  printtrace_diffnow(&s, "khash");

  s = trace_start();
  chash *ch = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_il(ch, vlit(key), vlit(value));
  }
  printtrace_diffnow(&s, "clib-l");

  s = trace_start();
  chash *ch2 = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  for (size_t i = 0; i < MAX; i++) {
    key = rand(), value = rand();
    chash_i(ch2, vlit(key), vlit(value));
  }
  printtrace_diffnow(&s, "clib-s");

  return EXIT_SUCCESS;
}
