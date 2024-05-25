#ifndef CHASH_H
#define CHASH_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const void *const TOMBSTONE =
    &"gqhDrC4f8B319YSccLSDG6yE4zD51bnFntgLHrbt132ejbz9TpeL9rMFxOe61DDdQ8WNETtjE"
     "YQKfmZFSqx7gbvmWnbYHNBOI6lS52QbUuUb1TGMj13V56HcjUEjJsvv";

typedef uint32_t key_ttt;
typedef struct {
  size_t s;
  size_t c;
  size_t n;
  key_ttt *ks;
  void *vs;
} chash;

static inline size_t hash_int(key_ttt x) { return x % 3; }

/*
 * chm value at index:
 * Get the index in the data structure of the hashmap `h`
 * of the value at position `i`
 */
#define chm_vatix(h, i) (i) * (h)->s
/*
 * chm key at index:
 * Get the index in the data structure of the hashmap `h`
 * of the key at position `i`
 */
#define chm_katix(h, i) (i)
/*
 * chm value at:
 * Get the value the hashmap `h` at position `i`
 * as `unsigned char*`
 */
#define chm_vat(h, i) ((unsigned char *)(h)->vs)[chm_vatix((h), (i))]
/*
 * chm key at index:
 * Get the key the hashmap `h` at position `i`
 * as `key_t`
 */
#define chm_kat(h, i) (h)->ks[i]

static inline void chash_i(chash *h, key_ttt k, void *value) {
  size_t i = hash_int(k) % h->n;
  printf("hash = %zu\n", i); // linear probing
  while (chm_vat(h, i) && chm_kat(h, i) != k)
    i++;
  printf("i = %zu\n", i);
  chm_kat(h, i) = k;
  memcpy(&chm_vat(h, i), value, h->s);
}

static inline void **chash_g(chash *h, key_ttt k) {
  size_t i = hash_int(k) % h->n;
  while (chm_kat(h, i) != k)
    i++;

  // void *r = (void*) &chm_vat(h, i);
  // return r; 
  return (void*) &chm_vat(h, i);
}

// static inline void chash_d(chash *h, key_ttt k) { size_t i = hash_int(k) % h->n; }

/*
 * chm typed value at index:
 * Get the value the hashmap `h` at position `i`
 * as `type *`
 * NOTE:
 */
#define chmt_vat(h, type, i) ((type *)(h)->vs)[(i)]
#define chasht_i(hm, k, type, value)                                           \
  do {                                                                         \
    size_t i = hash_int((k)) % (hm).n;                                         \
    printf("hash = %zu\n", i);                                                 \
    while (chmt_vat(&(hm), type, i) && chm_kat(&(hm), i) != (k))               \
      i++;                                                                     \
    printf("i = %zu\n", i);                                                    \
    chm_kat(&(hm), i) = k;                                                     \
    chmt_vat(&(hm), type, i) = value;                                          \
  } while (0)

// static inline void chash_d(chash *hm, key_t k) {
//   size_t i = hash_int(k) % hm->n;
//   while (chm_kat(hm, i) != k)
//     i++;
//   chm_vat(hm, i) = TOMBSTONE;
//
// }

[[nodiscard]] static inline chash chash_init(size_t size, size_t nmemb) {
  return (chash){.s = size,
                 .c = 0,
                 .n = nmemb,
                 .ks = malloc(sizeof(key_ttt) * nmemb),
                 .vs = malloc(size * nmemb)};
}

#endif
