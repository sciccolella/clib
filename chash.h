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

#define chm_vat(h, i) ((unsigned char *)(h)->vs)[(i) * (h)->s]
#define chm_kat(h, i) (h)->ks[i]
#define chm_vati(h, i) (i) * (h).s

static inline void chash_i(chash *h, key_t k, void *value) {
  size_t i = hash_int(k) % h->n;
  // linear probing
  while (chm_vat(h, i) && chm_kat(h, i) != k)
    i++;
  printf("i = %zu\n", i);
  chm_kat(h, i) = k;
  memcpy(&chm_vat(h, i), value, h->s);
}

// static inline void chash_d(chash *hm, key_t k) {
//   size_t i = hash_int(k) % hm->n;
//   while (chm_kat(hm, i) != k)
//     i++;
//   chm_vat(hm, i) = TOMBSTONE;
//
// }

#define chmt_vat(h, type, i) ((type *)(h)->vs)[(i)]
#define chasht_i(hm, k, type, value)                                           \
  do {                                                                         \
    size_t i = hash_int((k)) % (hm).n;                                         \
    while (chmt_vat(&(hm), type, i) && chm_kat(&(hm), i) != (k))                 \
      i++;                                                                     \
    printf("i = %zu\n", i);                                                    \
    chm_kat(&(hm), i) = k;                                                     \
    chmt_vat(&(hm), type, i) = value;                                          \
  } while (0)

[[nodiscard]]
static inline chash chash_init(size_t size, size_t nmemb) {
  return (chash){.s = size,
                 .c = 0,
                 .n = nmemb,
                 .ks = malloc(sizeof(key_t) * nmemb),
                 .vs = malloc(size * nmemb)};
}

#endif
