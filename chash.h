#ifndef CHASH_H
#define CHASH_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const void *const TOMBSTONE =
    &"gqhDrC4f8B319YSccLSDG6yE4zD51bnFntgLHrbt132ejbz9TpeL9rMFxOe61DDdQ8WNETtjE"
     "YQKfmZFSqx7gbvmWnbYHNBOI6lS52QbUuUb1TGMj13V56HcjUEjJsvv";

typedef uint32_t key_t;
typedef struct {
  size_t s;
  size_t c;
  size_t n;
  key_t *ks;
  void *vs;
} chash;

static inline size_t hash_int(key_t x) { return x % 3; }

// #define vs_at(vs, s, i) ((unsigned char *)(vs))[(i) * (s)]
#define chash_vat(h, i) ((unsigned char *)(h)->vs)[(i) * (h)->s]

static inline void chash_i(chash *h, key_t k, void *value) {
  size_t i = hash_int(k) % h->n;
  // linear probing
  while (chash_vat(h, i) && h->ks[i] != k)
    i++;
  printf("i = %zu\n", i);
  h->ks[i] = k;
  memcpy(&chash_vat(h, i), value, h->s);
}

#define chasht_i(h, k, type, value)                                            \
  do {                                                                         \
    size_t i = hash_int(k) % (h).n;                                            \
    while (chash_vat((&h), i))                                                 \
      i++;                                                                     \
    printf("i = %zu\n", i);                                                    \
    ((type *)((h)).vs)[(i)] = (value);                                         \
  } while (0)

[[nodiscard]] static inline chash chash_init(size_t size, size_t nmemb) {
  return (chash){.s = size,
                 .c = 0,
                 .n = nmemb,
                 .ks = malloc(sizeof(key_t) * nmemb),
                 .vs = malloc(size * nmemb)};
}

#endif
