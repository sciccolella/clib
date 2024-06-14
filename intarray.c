#include "immintrin.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "benchmark/tracing.h"

// https://stackoverflow.com/a/26012188
void p256_hex_u32(__m256i x) {
  uint32_t v[8] = {0};
  _mm256_storeu_si256((__m256i *)v, x);
  printf("%08x %08x %08x %08x %08x %08x %08x %08x\n", v[0], v[1], v[2], v[3],
         v[4], v[5], v[6], v[7]);
}

int main(int argc, char *argv[]) {
  trace_t s;
  srand(21);
  size_t VSIZE = 2<<29;
  uint32_t *v = calloc(VSIZE, sizeof(v));
  for (size_t i = 0; i < VSIZE; i++) {
    v[i] = rand();
  }
  v[rand()%VSIZE] = 21;
  size_t pos = 0;

  s = trace_start();
  __m256i needle = _mm256_set1_epi32(21);
  for (size_t i = 0; i < VSIZE; i += 8) {
    __m256i a = _mm256_lddqu_si256((const __m256i *)&v[i]);
    __m256i cmp = _mm256_cmpeq_epi32(a, needle); // CPI 0.5
    // p256_hex_u32(cmp);
    uint32_t mask = _mm256_movemask_epi8(cmp); // CPI 1
    // printf("mask = %32b\n", mask);
    if (mask) {
      pos = i + (__builtin_ctz(mask) >> 2);
      break;
    }
  }
  printtrace_diffnow(&s, "intr");
  printf("pos = %zu\n", pos);

  // s = trace_start();
  // needle = _mm256_set1_epi32(21);
  // for (size_t i = 0; i < VSIZE; i += 16) {
  //   __m256i a0 = _mm256_lddqu_si256((const __m256i *)&v[i]);
  //   __m256i a1 = _mm256_lddqu_si256((const __m256i *)&v[i+8]);
  //   __m256i cmp0 = _mm256_cmpeq_epi32(a0, needle); // CPI 0.5
  //   __m256i cmp1 = _mm256_cmpeq_epi32(a1, needle); // CPI 0.5
  //   // p256_hex_u32(cmp);
  //   uint32_t mask0 = _mm256_movemask_epi8(cmp0); // CPI 1
  //   uint32_t mask1 = _mm256_movemask_epi8(cmp1); // CPI 1
  //   // printf("mask = %32b\n", mask);
  //   if (mask0 | mask1) {
  //     // FIX: this is wrong
  //
  //     pos = i + (__builtin_ctz(mask0) >> 2);
  //     pos = i + 8 + (__builtin_ctz(mask1) >> 2);
  //     break;
  //   }
  //   if (mask0) {
  //     pos = i + (__builtin_ctz(mask0) >> 2);
  //     break;
  //   }
  // }
  // printtrace_diffnow(&s, "intr2");
  // printf("pos = %zu\n", pos);

  s = trace_start();
  pos = 0;
  for (size_t i = 0; i < VSIZE; i++) {
    if (v[i] == 21) {
      pos = i;
      break;
    }
  }
  printtrace_diffnow(&s, "for");
  printf("pos = %zu\n", pos);

  return EXIT_SUCCESS;
}
