#include "immintrin.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// https://stackoverflow.com/a/26012188
void p256_hex_u32(__m256i x) {
  uint32_t v[8] = {0};
  _mm256_storeu_si256((__m256i *)v, x);
  printf("%08x %08x %08x %08x %08x %08x %08x %08x\n", v[0], v[1], v[2], v[3],
         v[4], v[5], v[6], v[7]);
}

int main(int argc, char *argv[]) {
  srand(21);
  size_t VSIZE = 64;
  uint32_t *v = calloc(VSIZE, sizeof(v));
  for (size_t i = 0; i < VSIZE; i++) {
    v[i] = rand();
  }
  v[2] = 21;
  size_t pos = 0;
  __m256i check = _mm256_set1_epi32(21);
  for (size_t i = 0; i < VSIZE; i += 8) {
    __m256i a = _mm256_lddqu_si256((const __m256i *)&v[i]);
    __m256i cmp = _mm256_cmpeq_epi32(a, check); //CPI 0.5
    p256_hex_u32(cmp);
    uint8_t mask = _mm256_movemask_epi8(cmp);
    printf("mask = %d\n", mask);
    // printf("__builtin_ctz(~mask = %d\n", __builtin_ctz(~mask));
    // printf("%d\n", __builtin_ctz(~mask));
  }
  printf("pos = %zu\n", pos);

  return EXIT_SUCCESS;
}
