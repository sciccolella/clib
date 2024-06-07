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

typedef uint32_t key_ty;
typedef struct {
  size_t sk;
  size_t sv;
  size_t n;
  size_t c;
  size_t mod;
  void *ks;
  void *vs;
} chash;

static inline size_t chash_resize2(chash *h, size_t nmemb);

[[nodiscard]] static inline chash *chash_init(size_t ksize, size_t vsize) {
  size_t nmemb = 1 << 3;
  chash *h = malloc(sizeof *h);
  if (!h)
    return NULL;
  h->sk = ksize;
  h->sv = vsize;
  h->n = 0;
  h->c = nmemb;
  h->mod = (h->c) - 1;
  h->ks = calloc(nmemb, sizeof(key_ty));
  h->vs = calloc(nmemb, h->sv);
  // h->ks = malloc(h->sk * nmemb);
  // h->vs = malloc(h->sv * nmemb);
  return h;
}

static inline void chash_destroy(chash *h) {
  FREE(h->ks);
  FREE(h->vs);
  FREE(h);
}

static inline size_t hash_int(key_ty x) { return x % 3; }

/* chm value at index:
 * Get the index in the data structure of the hashmap `h`
 * of the value at position `i` */
#define chm_vatix(h, i) ((i) * (h)->sv)

/* chm key at index:
 * Get the index in the data structure of the hashmap `h`
 * of the key at position `i` */
#define chm_katix(h, i) ((i) * (h)->sk)

/* chm value at as void*:
 * Get the value the hashmap `h` at position `i` as `void*` */
#define chm_vat(h, i) (void *)&((unsigned char *)(h)->vs)[chm_vatix((h), (i))]

/* chm key at index:
 * Get the key the hashmap `h` at position `i` as `key_t` */
#define chm_kat(h, i) (void *)&((unsigned char *)(h)->ks)[chm_katix((h), (i))]

/* sized value at index:
 * Get the index in the vector of the value of size `size` at position `i` */
#define s_vatix(i, size) i *size
// TODO:document
#define s_katix(i, size) i *size

/* sized key at:
 * Get the key in `ks` at position `i` */
#define s_kat(ks, i, size)                                                     \
  (void *)&((unsigned char *)(ks))[s_katix((i), (size))]

/* sized vale at as void*:
 * Get the value of size `size` in `vs` at position `i` as void pointer */
#define s_vat(vs, i, size)                                                     \
  (void *)&((unsigned char *)(vs))[s_vatix((i), (size))]

// TODO: FIXME
#define s_kat_null(ks, i, size) s_kat((ks), (i), (size)) == NULL
#define chm_kat_null(hm, i) chm_kat((hm), i) == NULL

// [[deprecated]] static inline void chash_i(chash *h, key_ty k, void *value) {
//   size_t i = hash_int(k) % h->c;
//   printf("key = %d, hash = %zu\n", k, i); // linear probing
//   // while (chm_vat(h, i) && chm_kat(h, i) != k)
//   while (chm_vat(h, i) && memcmp((void *)&chm_vat(h, i), &TOMBSTONE, h->s) &&
//          chm_kat(h, i) != k && i < h->c)
//     i++;
//
//   // printf("i = %zu\n", i);
//   chm_kat(h, i) = k;
//   memcpy(&chm_vat(h, i), value, h->s);
//   h->n++;
// }

static inline int keql(void *a, void *b) {
  uint32_t A = *(uint32_t *)a;
  uint32_t B = *(uint32_t *)b;
  // printf("[%s] A:%10u B:%10u eql?: %d\n", __func__, A, B, A == B);
  return A == B;
}

static inline int memzero(void *m, size_t s) {
  size_t r = 0;
  uint8_t *m_ = m;
  for (size_t i = 0; i < s; i++) {
    r += m_[i];
  }
  return r == 0;
}

static inline void chash_i2(chash *h, key_ty k, void *value) {
  if (h->n == h->c)
    if (!chash_resize2(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = hash_int(k) & h->mod;
  printf("[%s] k:%3u h:%3zu\n", __func__, k, i);
  // for (size_t x = 0; x < h->c; x++) {
  //   printf("%3zu: kat:%20p\t*kat:%10u\tkat_null?:%d", x, chm_kat(h, x),
  //          *(uint32_t *)chm_kat(h, x), chm_kat_null(h, x));
  //
  //   printf("\t t:%d", memzero(chm_kat(h, x), h->sk));
  //   printf("\n\t chm_vat:%d\t memzero:%d", chm_vat(h, x),  memzero(chm_kat(h,
  //   x), h->sk));
  //   // printf("\t%d", memcmp(chm_kat(h, x), NULL, h->sk));
  //   puts("");
  // }
  // printf("key = %d, hash = %zu\n", k, i); // linear probing
  // while (!(chm_kat_null(h, i)) && chm_kat(h, i) != k && i < h->c &&
  while (
      // !(chm_kat_null(h, i))
      !memzero(chm_kat(h, i), h->sk) &&
      !keql(chm_kat(h, i), &k)
      // chm_kat(h, i) != k
      && i < h->c
      // chm_vat(h, i)
      && !memzero(chm_vat(h, i), h->sv)

  ) {
    i = (i + 1) & h->mod;
  }

  printf("\tinserted k:%u -> %zu\n", k, i);
  // printf("i = %zu\n", i);
  // chm_kat(h, i) = k;
  memcpy(chm_kat(h, i), &k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}

static inline void **chash_g(chash *h, key_ty k) {
  printf("[%s] k:%5u\n", __func__, k);
  size_t i = hash_int(k) & h->mod;
  size_t last = i;
  while (!keql(chm_kat(h, i), &k)) {
    i = (i + 1) & h->mod;
    if (i == last)
      return NULL;
  }
  return chm_vat(h, i);
}
#define chash_gt(h, k, type)                                                   \
  ({                                                                           \
    type *g = (type *)chash_g((h), (k));                                       \
    (g) ? *g : NULL;                                                           \
  })

// [[deprecated]] static inline void chash_d(chash *h, key_ty k) {
//   size_t i = hash_int(k) % h->c;
//   while (chm_kat(h, i) != k && i < h->c)
//     i++;
//   /* TODO: reset key to proper null value */
//
//   chm_kat(h, i) = 1;
//   memcpy((void *)&chm_vat(h, i), &TOMBSTONE, h->s);
//   h->n--;
// }

static inline void chash_d2(chash *h, key_ty k) {
  size_t i = hash_int(k) & h->mod;
  size_t last = i;
  while (!keql(chm_kat(h, i), &k)) {
    i = (i + 1) & h->mod;
    if (i == last)
      return;
  }

  // for (size_t x = 0; x < h->c; x++) {
  //   printf("%3zu: kat:%20p\t*kat:%10u\t", x, chm_kat(h, x),
  //          *(uint32_t *)chm_kat(h, x));
  //
  //   printf("\t!k_mem0:%d", !memzero(chm_kat(h, x), h->sk));
  //   printf("\t!v_mem0:%d", !memzero(chm_vat(h, x), h->sv));
  //   printf("\twhile:%d",
  //          memzero(chm_kat(h, x), h->sk) && memzero(chm_vat(h, x),
  //          h->sv));
  //   puts("");
  // }
  // printf("key = %d, hash = %zu\n", k, i); // linear probing
  size_t j = (i + 1) & h->mod;
  //  NOTE: if i arrives to last position, then j will do another round
  //  across the array, this is not unexpected since it may be possible that
  //  a value has been inserted considerint it as a circular array.
  //  If we avoid this option (also in insertion) then the cycle can be stopped
  //  when i reaches h->c.
  //  As it is, this cycle is "wasting" at most h->c iterations,
  //  only if the map if full
  while (j != i &&
         !(memzero(chm_kat(h, j), h->sk) && memzero(chm_vat(h, j), h->sv))) {
    // HACK: this need to be changed to get an actual hash func
    size_t hj = hash_int(*(uint32_t *)chm_kat(h, j)) & h->mod;
    // printf("\t[i:%2zu] j:%2zu, not null\n", i, j);
    // printf("\t\thj = %zu\n", hj);
    // printf("\t\tj<i = %d\n", j < i);
    // printf("\t\thj <= i = %d\n", hj <= i);
    // printf("\t\thj>j = %d\n", hj > j);
    if ((j < i) ^ (hj <= i) ^ (hj > j)) {
      printf("[%s], substuting %2zu<-%2zu\n", __func__, i, j);
      memcpy(chm_kat(h, i), chm_kat(h, j), h->sk);
      // chm_kat(h, i) = chm_kat(h, j);
      memcpy(chm_vat(h, i), chm_vat(h, j), h->sv);
      i = j;
    }
    j = (j + 1) & h->mod;
  }
  memset(chm_kat(h, i), 0, h->sk);
  memset(chm_vat(h, i), 0, h->sv);
  h->n--;
}

// [[deprecated]] static inline size_t chash_resize(chash **h, size_t size) {
//   printf("[%s]\n", __func__);
//   if (size < (*h)->c)
//     return 0;
//
//   chash *h_ = chash_init((*h)->s, size);
//   for (size_t i = 0; i < (*h)->c; i++) {
//     printf("i = %zu\n", i);
//     if (!(chm_kat_null((*h), i)))
//       chash_i2(h_, chm_kat((*h), i), chm_vat((*h), i));
//   }
//   chash_destroy(*h);
//   *h = h_;
//   return size;
// }

static inline size_t chash_resize2(chash *h, size_t nmemb) {
  printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  key_ty *nks = calloc(nmemb, sizeof(key_ty));
  void *nvs = calloc(nmemb, h->sv);
  if (!nks || !nvs)
    return 0;

  key_ty *oks = h->ks;
  void *ovs = h->vs;
  h->ks = nks;
  h->vs = nvs;
  h->n = 0;
  for (size_t i = 0; i < h->c; i++) {
    if (!(memzero(s_kat(oks, i, h->sk), h->sk) &&
          memzero(s_vat(ovs, i, h->sv), h->sv)))
      // HACK: change this to actual key
      chash_i2(h, *(uint32_t *)s_kat(oks, i, h->sk), s_vat(ovs, i, h->sv));
  }
  FREE(oks);
  FREE(ovs);
  h->c = nmemb;
  h->mod = h->c - 1;
  return nmemb;
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
