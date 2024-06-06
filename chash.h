#ifndef CHASH_H
#define CHASH_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FREE
#define FREE(x)                                                                \
  do {                                                                         \
    free((x));                                                                 \
    (x) = NULL;                                                                \
  } while (0)
#endif

static void *const TOMBSTONE =
    &"gqhDrC4f8B319YSccLSDG6yE4zD51bnFntgLHrbt132ejbz9TpeL9rMFxOe61DDdQ8WNETtjE"
     "YQKfmZFSqx7gbvmWnbYHNBOI6lS52QbUuUb1TGMj13V56HcjUEjJsvv";

typedef uint32_t key_ty;
typedef struct {
  size_t s;
  size_t n;
  size_t c;
  key_ty *ks;
  void *vs;
} chash;

[[nodiscard]] static inline chash *chash_init(size_t size, size_t nmemb) {
  chash *h = malloc(sizeof *h);
  if (!h)
    return NULL;
  h->s = size;
  h->n = 0;
  h->c = nmemb;
  h->ks = malloc(sizeof(key_ty) * nmemb);
  h->vs = malloc(size * nmemb);
  return h;
}

static inline void chash_destroy(chash *h) {
  FREE(h->ks);
  FREE(h->vs);
  FREE(h);
}

static inline size_t hash_int(key_ty x) { return x % 3; }

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
#define chm_vat_vp(h, i)                                                       \
  (void *)&((unsigned char *)(h)->vs)[chm_vatix((h), (i))]
/*
 * chm key at index:
 * Get the key the hashmap `h` at position `i`
 * as `key_t`
 */
#define chm_kat(h, i) (h)->ks[i]

#define chm_kat_null(hm, i) chm_kat((hm), i) == 0

[[deprecated]] static inline void chash_i(chash *h, key_ty k, void *value) {
  size_t i = hash_int(k) % h->c;
  printf("key = %d, hash = %zu\n", k, i); // linear probing
  // while (chm_vat(h, i) && chm_kat(h, i) != k)
  while (chm_vat(h, i) && memcmp((void *)&chm_vat(h, i), &TOMBSTONE, h->s) &&
         chm_kat(h, i) != k && i < h->c)
    i++;

  // printf("i = %zu\n", i);
  chm_kat(h, i) = k;
  memcpy(&chm_vat(h, i), value, h->s);
  h->n++;
}

static inline void chash_i2(chash *h, key_ty k, void *value) {
  size_t i = hash_int(k) % h->c;
  printf("key = %d, hash = %zu\n", k, i); // linear probing
  while (!(chm_kat_null(h, i)) && chm_kat(h, i) != k && i < h->c &&
         // NOTE: chm_vat condition may be removed
         chm_vat(h, i))
    i++;

  if (i == h->c) {
    // TODO: resize & recall func
    printf("full\n");
    return;
  }
  // printf("i = %zu\n", i);
  chm_kat(h, i) = k;
  memcpy(&chm_vat(h, i), value, h->s);
  h->n++;
}

static inline void **chash_g(chash *h, key_ty k) {
  size_t i = hash_int(k) % h->c;
  while (chm_kat(h, i) != k && !(chm_kat_null(h, i)) && i < h->c)
    i++;

  return (void *)&chm_vat(h, i);
}

[[deprecated]] static inline void chash_d(chash *h, key_ty k) {
  size_t i = hash_int(k) % h->c;
  while (chm_kat(h, i) != k && i < h->c)
    i++;
  /* TODO: reset key to proper null value */
  chm_kat(h, i) = 1;
  memcpy((void *)&chm_vat(h, i), &TOMBSTONE, h->s);
  h->n--;
}

static inline void chash_d2(chash *h, key_ty k) {
  size_t i = hash_int(k) % h->c;
  while (chm_kat(h, i) != k && i < h->c)
    i++;
  size_t hi = hash_int(chm_kat(h, i)) % h->c;
  size_t j = i + 1;
  // memcpy((void *)&chm_vat(h, i), &TOMBSTONE, h->s);
  // TODO: add stop if j>i
  while (!(chm_kat_null(h, j))) {
    size_t hj = hash_int(chm_kat(h, j)) % h->c;
    printf("\tj:%zu, not null\n", j);
    printf("\t\thj = %zu\n", hj);
    printf("\t\thi = %zu\n", hi);
    printf("\t\tj<i = %d\n", j < i);
    printf("\t\thj <= i = %d\n", hj <= i);
    printf("\t\thj>j = %d\n", hj > j);
    if ((j < i) ^ (hj <= i) ^ (hj > j)) {
      printf("[%s], substuting %2zu<-%2zu\n", __func__, i, j);
      chm_kat(h, i) = chm_kat(h, j);
      // memcpy((void *)&chm_vat(h, i), (void *)&chm_vat(h, j), h->s);
      memcpy(chm_vat_vp(h, i), chm_vat_vp(h, j), h->s);
      i = j;
      hi = hj;
    }
    j++;
  }
  /* TODO: reset key to proper null value */
  chm_kat(h, i) = 0;
  memset(&chm_vat(h, i), 0, h->s);
  h->n--;
}

static inline size_t chash_resize(chash **h, size_t size) {
  printf("[%s]\n", __func__);
  if (size < (*h)->c)
    return 0;

  chash *h_ = chash_init((*h)->s, size);
  for (size_t i = 0; i < (*h)->c; i++) {
    printf("i = %zu\n", i);
    if (!(chm_kat_null((*h), i)))
      chash_i2(h_, chm_kat((*h), i), chm_vat_vp((*h), i));
  }
  chash_destroy(*h);
  *h = h_;
  return size;
}

/*
 * chm typed value at index:
 * Get the value the hashmap `h` at position `i`
 * as `type *`
 */
#define chmt_vat(h, type, i) ((type *)(h)->vs)[(i)]
#define chasht_i(hm, k, vtype, value)                                          \
  do {                                                                         \
    size_t i = hash_int((k)) % (hm).n;                                         \
    printf("hash = %zu\n", i);                                                 \
    /* while (chmt_vat(&(hm), vtype, i) && chm_kat(&(hm), i) != (k)) */        \
    while (chmt_vat(&(hm), vtype, i) &&                                        \
           chmt_vat(&(hm), vtype, i) != TOMBSTONE &&                           \
           chm_kat(&(hm), i) != (k) && i < (hm).n)                             \
      i++;                                                                     \
    printf("i = %zu\n", i);                                                    \
    chm_kat(&(hm), i) = k;                                                     \
    chmt_vat(&(hm), vtype, i) = value;                                         \
    (hm).c++;                                                                  \
  } while (0)

#define chasht_g(hm, k, vtype, value)                                          \
  do {                                                                         \
    size_t i = hash_int((k)) % (hm).n;                                         \
    while (chm_kat(&(hm), i) != k && !(chm_kat_null(&(hm), i)) && i < (hm).n)  \
      i++;                                                                     \
    if (chm_kat_null(&(hm), i))                                                \
      (value) = (vtype)0;                                                      \
    else                                                                       \
      (value) = ((vtype *)(hm).vs)[i];                                         \
  } while (0)

#define chasht_d(hm, k, vtype)                                                 \
  do {                                                                         \
    size_t i = hash_int((k)) % (hm).n;                                         \
    while (chm_kat(&(hm), i) != k && i < (hm).n)                               \
      i++;                                                                     \
    /* TODO: reset key to proper null value */                                 \
    chm_kat(&(hm), i) = 0;                                                     \
    chmt_vat(&(hm), vtype, i) = TOMBSTONE;                                     \
    (hm).c--;                                                                  \
  } while (0)

#define chasht_resize(hm, vtype, size)                                         \
  do {                                                                         \
    size_t newc = size;                                                        \
    if ((hm).n < newc) {                                                       \
      chash nhm = chash_init((hm).s, newc);                                    \
      for (size_t x = 0; x < (hm).n; x++) {                                    \
        chasht_i(nhm, chm_kat(&(hm), x), vtype, chmt_vat(&(hm), vtype, x));    \
      }                                                                        \
      /* TODO: free hm */                                                      \
      hm = nhm;                                                                \
    }                                                                          \
  } while (0)

#endif
