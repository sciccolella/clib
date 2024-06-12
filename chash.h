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

typedef struct {
  int (*keql)(void *, void *);
  size_t (*khash)(void *);
  size_t sk;
  size_t sv;
  size_t n;
  size_t c;
  size_t mod;
  void *ds;
} chash;

typedef uint32_t key_ty;
static inline size_t hash_int(key_ty x) { return x % 3; }
static inline size_t hash_int2(void *x) {
  uint32_t X = *(uint32_t *)x;
  // X ^= (X >> 33);
  // X *= 0xff51afd7ed558ccd;
  // X ^= (X >> 33);
  // X *= 0xc4ceb9fe1a85ec53;
  // X ^= (X >> 33);
  X ^= X >> 16;
  X *= 0x85ebca6b;
  X ^= X >> 13;
  X *= 0xc2b2ae35;
  X ^= X >> 16;
  return X;
  // return X % 3;
  // return X;
}
static inline int keql(void *a, void *b) {
  uint32_t A = *(uint32_t *)a;
  uint32_t B = *(uint32_t *)b;
  // printf("[%s] A:%10u B:%10u eql?: %d\n", __func__, A, B, A == B);
  return A == B;
}
static inline size_t chash_resize(chash *h, size_t nmemb);

[[nodiscard]] static inline chash *chash_init(size_t ksize, size_t vsize) {
  size_t nmemb = 1 << 3;
  // size_t nmemb = 1 << 10;
  chash *h = malloc(sizeof *h);
  if (!h)
    return NULL;
  // TODO: add to initialization
  h->keql = keql;
  h->khash = hash_int2;
  h->sk = ksize;
  h->sv = vsize;
  h->n = 0;
  h->c = nmemb;
  h->mod = (h->c) - 1;
  h->ds = calloc(nmemb, h->sk + h->sv);
  return h;
}

static inline void chash_destroy(chash *h) {
  FREE(h->ds);
  FREE(h);
}

/* chm value at index:
 * Get the index in the data structure of the hashmap `h`
 * of the value at position `i` */
#define chm_vatix(h, i) ((i) * ((h)->sk + (h)->sv) + (h)->sk)

/* chm key at index:
 * Get the index in the data structure of the hashmap `h`
 * of the key at position `i` */
#define chm_katix(h, i) ((i) * ((h)->sk + (h)->sv))

/* chm value at as void*:
 * Get the value the hashmap `h` at position `i` as `void*` */
#define chm_vat(h, i) (void *)&((unsigned char *)(h)->ds)[chm_vatix((h), (i))]

/* chm key at index:
 * Get the key the hashmap `h` at position `i` as `key_t` */
#define chm_kat(h, i) (void *)&((unsigned char *)(h)->ds)[chm_katix((h), (i))]

/* sized value at index:
 * Get the index in the vector of the value of size `size` at position `i` */
#define s_vatix(i, ksize, vsize) i *(ksize + vsize) + ksize
// TODO:document
#define s_katix(i, ksize, vsize) i *(ksize + vsize)

/* sized key at:
 * Get the key in `ks` at position `i` */
#define s_kat(ks, i, ksize, vsize)                                             \
  (void *)&((unsigned char *)(ks))[s_katix((i), (ksize), (vsize))]

/* sized vale at as void*:
 * Get the value of size `size` in `vs` at position `i` as void pointer */
#define s_vat(vs, i, ksize, vsize)                                             \
  (void *)&((unsigned char *)(vs))[s_vatix((i), (ksize), (vsize))]

// TODO:document
#define s_kat_null(ks, i, ksize, vsize)                                        \
  s_kat((ks), (i), (ksize), (vsize)) == NULL
// TODO:document
#define chm_kat_null(hm, i) chm_kat((hm), i) == NULL

static inline int memzero(void *m, size_t s) {
  size_t r = 0;
  uint8_t *m_ = m;
  for (size_t i = 0; i < s; i++) {
    r += m_[i];
  }
  return r == 0;
}

static inline void chash_i(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }
  // if (h->n == h->c)
  if (h->n >= ((double)h->c * 0.85))
    if (!chash_resize(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = (h->khash)(k)&h->mod;
  // printf("[%s] k:%3u h:%3zu\n", __func__, *(uint32_t *)k, i);
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
  while (!memzero(chm_kat(h, i), h->sk) && !memzero(chm_vat(h, i), h->sv) &&
         // !(h->keql)(chm_kat(h, i), k)
         // !(*(uint32_t*)k == *(uint32_t*)chm_kat(h, i))
         memcmp(k, chm_kat(h, i), h->sk))
  // do not format
  {
    i = (i + 1) & h->mod;
  }

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}

#define chash_ikl(h, k, value) chash_i((h), &(typeof((k))){(k)}, (value))

static inline void **chash_g(chash *h, void *k) {
  // printf("[%s] k:%5u\n", __func__, *(uint32_t *)k);
  size_t i = (h->khash)(k)&h->mod;
  size_t last = i;
  while (!(h->keql)(chm_kat(h, i), k)) {
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

static inline void chash_d(chash *h, void *k) {
  size_t i = (h->khash)(k)&h->mod;
  size_t last = i;
  while (!(h->keql)(chm_kat(h, i), k)) {
    i = (i + 1) & h->mod;
    if (i == last)
      return;
  }

  size_t j = (i + 1) & h->mod;
  //  NOTE: if i arrives to last position, then j will do another round
  //  across the array, this is not unexpected since it may be possible that
  //  a value has been inserted considerint it as a circular array.
  //  As it is, this cycle is "wasting" at most h->c iterations,
  //  only if the map if full
  while (j != i &&
         !(memzero(chm_kat(h, j), h->sk) && memzero(chm_vat(h, j), h->sv))) {
    size_t hj = (h->khash)(chm_kat(h, j)) & h->mod;
    // printf("\t[i:%2zu] j:%2zu, not null\n", i, j);
    // printf("\t\thj = %zu\n", hj);
    // printf("\t\tj<i = %d\n", j < i);
    // printf("\t\thj <= i = %d\n", hj <= i);
    // printf("\t\thj>j = %d\n", hj > j);
    if ((j < i) ^ (hj <= i) ^ (hj > j)) {
      // printf("[%s], substuting %2zu<-%2zu\n", __func__, i, j);
      memcpy(chm_kat(h, i), chm_kat(h, j), h->sk);
      memcpy(chm_vat(h, i), chm_vat(h, j), h->sv);
      i = j;
    }
    j = (j + 1) & h->mod;
  }
  memset(chm_kat(h, i), 0, h->sk);
  memset(chm_vat(h, i), 0, h->sv);
  h->n--;
}

static inline size_t chash_resize(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  h->ds = nds;
  h->n = 0;
  for (size_t i = 0; i < h->c; i++) {
    if (!(memzero(s_kat(ods, i, h->sk, h->sv), h->sk) &&
          memzero(s_vat(ods, i, h->sk, h->sv), h->sv)))
      // HACK: change this to actual key
      chash_i(h, s_kat(ods, i, h->sk, h->sv), s_vat(ods, i, h->sk, h->sv));
  }
  FREE(ods);
  h->c = nmemb;
  h->mod = h->c - 1;
  return nmemb;
}

#endif
