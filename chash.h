#ifndef CHASH_H
#define CHASH_H

#include "immintrin.h"
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

#define CHM_MAXLOAD 0.65
enum { CHM_FK, CHM_FZ };
typedef struct chash chash;
struct chash {
  int (*keql)(void *, void *);
  size_t (*khash)(void *);
  size_t (*fkz)(chash *, void *, uint8_t *);
  size_t (*fz)(chash *, void *);
  size_t sk;
  size_t sv;
  size_t n;
  size_t c;
  size_t mod;
  void *ds;
};

typedef uint32_t key_ty;
static inline size_t hash_debug(void *x) {
  uint32_t X = *(uint32_t *)x;
  return X % 3;
}
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
}
static inline int keql(void *a, void *b) {
  uint32_t A = *(uint32_t *)a;
  uint32_t B = *(uint32_t *)b;
  // printf("[%s] A:%10u B:%10u eql?: %d\n", __func__, A, B, A == B);
  return A == B;
}
static inline size_t chash_resize(chash *h, size_t nmemb);
static inline size_t chash_resizel(chash *h, size_t nmemb);
static inline size_t chash_resizeA(chash *h, size_t nmemb);
static inline size_t chash_resize_p(chash *h, size_t nmemb);

static inline size_t chm_fz_peq(chash *restrict h, void *restrict k);
static inline size_t chm_fkz_peq(chash *restrict h, void *restrict k,
                                 uint8_t *restrict retcode);
static inline size_t chm_fkz(chash *restrict h, void *restrict k,
                             uint8_t *restrict retcode);
static inline size_t chm_fkz_l(chash *restrict h, void *restrict k,
                               uint8_t *restrict retcode);
static inline size_t chm_fz(chash *restrict h, void *restrict k);
static inline size_t chm_fz_l(chash *restrict h, void *restrict k);
// to make LSP happy
// NOTE: this has to be deleted
#ifndef __AVX2__
#define __AVX2__
#endif
#ifndef __AVX512F__
#define __AVX512F__
#endif

#ifdef __AVX2__
static inline size_t chm_fkz_avx2_u4(chash *restrict h, void *restrict k,
                                     uint8_t *restrict ret);
static inline size_t chm_fkz_avx2_u8(chash *restrict h, void *restrict k,
                                     uint8_t *restrict ret);
static inline size_t chm_fz_avx2_u4(chash *restrict h, void *restrict k);
static inline size_t chm_fz_avx2_u8(chash *restrict h, void *restrict k);
#endif
#ifdef __AVX512F__
static inline size_t chm_fkz_avx512f_u4(chash *restrict h, void *restrict k,
                                        uint8_t *restrict ret);
static inline size_t chm_fkz_avx512f_u8(chash *restrict h, void *restrict k,
                                        uint8_t *restrict ret);
// static inline size_t chm_fz_avx512f_u4(chash *restrict h, void *restrict k);
// static inline size_t chm_fz_avx512f_u8(chash *restrict h, void *restrict k);
#endif

[[nodiscard]] static inline chash *chash_init(size_t ksize, size_t vsize) {
  size_t nmemb = 1 << 4;
  // size_t nmemb = 1 << 10;
  chash *h = malloc(sizeof *h);
  if (!h)
    return NULL;
  // TODO: add to initialization
  h->keql = keql;
#ifdef CHDEBUG
  h->khash = hash_debug;
#else
  h->khash = hash_int2;
#endif
  h->sk = ksize;
  h->sv = vsize;
  h->fkz = chm_fkz_l;
#ifdef __AVX2__
  if (h->sv == 4 && h->sk == 4)
    h->fkz = chm_fkz_avx2_u4;
  if (h->sv == 8 && h->sk == 8)
    h->fkz = chm_fkz_avx2_u8;
#endif
#ifdef __AVX512F__
  if (h->sv == 4 && h->sk == 4)
    h->fkz = chm_fkz_avx512f_u4;
  if (h->sv == 8 && h->sk == 8)
    h->fkz = chm_fkz_avx512f_u8;
#endif
  h->fz = chm_fz_l;
#ifdef __AVX2__
  if (h->sv == 4 && h->sk == 4)
    h->fz = chm_fz_avx2_u4;
  if (h->sv == 8 && h->sk == 8)
    h->fz = chm_fz_avx2_u8;
#endif
#ifdef __AVX512F__
  // TODO: fix with fz
  if (h->sv == 4 && h->sk == 4)
    h->fkz = chm_fkz_avx512f_u4;
  if (h->sv == 8 && h->sk == 8)
    h->fkz = chm_fkz_avx512f_u8;
#endif
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
  // size_t r = 0;
  // uint8_t *m_ = m;
  // for (size_t i = 0; i < s; i++) {
  //   r += m_[i];
  // }
  // return r == 0;

  if (s == 4)
    return 0UL == *(uint32_t *)m;
  if (s == 8)
    return 0ULL == *(uint64_t *)m;
  uint8_t *m_ = m;
  for (size_t i = 0; i < s; i++) {
    if (m_[i])
      return 0;
  }
  return 1;
}

static inline void chash_i(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }

  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resize(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
static inline void chash_pi(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }

  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resize_p(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz_peq(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void chash_il(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }

  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resizel(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz_l(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void chash_iA(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }

  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resizeA(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = (h->fkz)(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}

// #define chash_ikl(h, k, value) chash_i((h), &(__typeof__((k))){(k)}, (value))

#ifdef __AVX2__
static inline size_t chash_resize_avx2_u4(chash *h, size_t nmemb);
static inline size_t chash_resize_avx2_u8(chash *h, size_t nmemb);
// TODO: debug remove
void p256_hex_u32(__m256i x, char *prefix) {
  uint32_t v[8] = {0};
  _mm256_storeu_si256((__m256i *)v, x);
  printf("[%s] %08x %08x %08x %08x %08x %08x %08x %08x\n", prefix, v[0], v[1],
         v[2], v[3], v[4], v[5], v[6], v[7]);
}

// TODO: debug remove
void p256_hex_u64(__m256i x, char *prefix) {
  uint64_t v[4] = {0};
  _mm256_storeu_si256((__m256i *)v, x);
  printf("[%s] %016lx %016lx %016lx %016lx\n", prefix, v[0], v[1], v[2], v[3]);
}

static inline size_t chm_fkz_avx2_u4(chash *restrict h, void *restrict k,
                                     uint8_t *restrict ret) {
  size_t i = (h->khash)(k)&h->mod;
  __m256i zeros = _mm256_setzero_si256();
  __m256i vk = _mm256_set1_epi32(*(uint32_t *)k);
avxinsert:
  for (; i < h->c - 4; i += 4) {
    // printf("i = %zu\n", i);
    __m256i a = _mm256_lddqu_si256((const __m256i *)chm_kat(h, i));
    __m256i cmpk = _mm256_cmpeq_epi32(a, vk);    // CPI 0.5
    __m256i cmpz = _mm256_cmpeq_epi32(a, zeros); // CPI 0.5
    // p256_hex_u32(a, "   a");
    // p256_hex_u32(cmpk, "cmpk");
    // p256_hex_u32(cmpz, "cmpz");

    uint32_t maskk = _mm256_movemask_epi8(cmpk); // CPI 1
    if (maskk) {
      // printf("%32b - CTZ=%4d\n", maskk, __builtin_ctz(maskk));
      i += (__builtin_ctz(maskk) >> 3);
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    uint32_t maskz = _mm256_movemask_epi8(cmpz); // CPI 1
    if (maskz) {
      // printf("%32b - CTZ=%4d\n", maskz, __builtin_ctz(maskz));
      i += (__builtin_ctz(maskz) >> 3);
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
  }

  for (; i < h->c; i++) {
    if (memcmp(k, chm_kat(h, i), h->sk) == 0) {
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_kat(h, i), h->sv)) {
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
    i = 0;
    goto avxinsert;
  }
out:
  // printf("%zu\n", problen);
  return i;
}

static inline size_t chm_fz_avx2_u4(chash *restrict h, void *restrict k) {
  size_t i = (h->khash)(k)&h->mod;
  __m256i zeros = _mm256_setzero_si256();
avxinsert:
  for (; i < h->c - 4; i += 4) {
    // printf("i = %zu\n", i);
    __m256i a = _mm256_lddqu_si256((const __m256i *)chm_kat(h, i));
    __m256i cmpz = _mm256_cmpeq_epi32(a, zeros); // CPI 0.5
    // p256_hex_u32(a, "   a");
    // p256_hex_u32(cmpk, "cmpk");
    // p256_hex_u32(cmpz, "cmpz");

    uint32_t maskz = _mm256_movemask_epi8(cmpz); // CPI 1
    if (maskz) {
      // printf("%32b - CTZ=%4d\n", maskz, __builtin_ctz(maskz));
      i += (__builtin_ctz(maskz) >> 3);
      goto out;
    }
  }

  for (; i < h->c; i++) {
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_vat(h, i), h->sv)) {
      goto out;
    }
    i = 0;
    goto avxinsert;
  }
out:
  // printf("%zu\n", problen);
  return i;
}
static inline size_t chm_fkz_avx2_u8(chash *restrict h, void *restrict k,
                                     uint8_t *restrict ret) {
  size_t i = (h->khash)(k)&h->mod;
  __m256i zeros = _mm256_setzero_si256();
  __m256i vk = _mm256_set1_epi64x(*(uint64_t *)k);
avxinsert:
  for (; i < h->c - 2; i += 2) {
    // printf("i = %zu\n", i);
    __m256i a = _mm256_lddqu_si256((const __m256i *)chm_kat(h, i));
    __m256i cmpk = _mm256_cmpeq_epi64(a, vk);    // CPI 0.5
    __m256i cmpz = _mm256_cmpeq_epi64(a, zeros); // CPI 0.5
    // p256_hex_u64(a, "   a");
    // p256_hex_u64(cmpk, "cmpk");
    // p256_hex_u64(cmpz, "cmpz");

    uint32_t maskk = _mm256_movemask_epi8(cmpk); // CPI 1
    if (maskk) {
      // printf("%32b - CTZ=%4d\n", maskk, __builtin_ctz(maskk));
      i += (__builtin_ctz(maskk) >> 4);
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    uint32_t maskz = _mm256_movemask_epi8(cmpz); // CPI 1
    if (maskz) {
      // printf("%32b - CTZ=%4d\n", maskz, __builtin_ctz(maskz));
      i += (__builtin_ctz(maskz) >> 4);
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
  }

  for (; i < h->c; i++) {
    if (memcmp(k, chm_kat(h, i), h->sk) == 0) {
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_kat(h, i), h->sv)) {
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
    i = 0;
    goto avxinsert;
  }
out:
  // printf("%zu\n", problen);
  return i;
}

static inline size_t chm_fz_avx2_u8(chash *restrict h, void *restrict k) {
  size_t i = (h->khash)(k)&h->mod;
  __m256i zeros = _mm256_setzero_si256();
avxinsert:
  for (; i < h->c - 2; i += 2) {
    // printf("i = %zu\n", i);
    __m256i a = _mm256_lddqu_si256((const __m256i *)chm_kat(h, i));
    __m256i cmpz = _mm256_cmpeq_epi64(a, zeros); // CPI 0.5
    // p256_hex_u64(a, "   a");
    // p256_hex_u64(cmpk, "cmpk");
    // p256_hex_u64(cmpz, "cmpz");

    uint32_t maskz = _mm256_movemask_epi8(cmpz); // CPI 1
    if (maskz) {
      // printf("%32b - CTZ=%4d\n", maskz, __builtin_ctz(maskz));
      i += (__builtin_ctz(maskz) >> 4);
      goto out;
    }
  }

  for (; i < h->c; i++) {
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_vat(h, i), h->sv)) {
      goto out;
    }
    i = 0;
    goto avxinsert;
  }
out:
  // printf("%zu\n", problen);
  return i;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void chash_i_avx2_u4(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }

  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resize_avx2_u4(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz_avx2_u4(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chash_resize_avx2_u4(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  size_t oc = h->c;
  void *ods = h->ds;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  for (size_t i = 0; i < oc; i++) {
    void *k =s_kat(ods, i, h->sk, h->sv);
    void *v = s_vat(ods, i, h->sk, h->sv);
    if (!(memzero(k, h->sk) &&
          memzero(v, h->sv))) {
      size_t pos = chm_fz_avx2_u4(h, k);
      memcpy(chm_kat(h, pos), k, h->sk);
      memcpy(chm_vat(h, pos), v, h->sv);
      h->n++;
    }
  }
  FREE(ods);
  return nmemb;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void **chash_g_avx2_u4(chash *h, void *k) {
  uint8_t retcode = CHM_FZ;
  size_t i = chm_fkz_avx2_u4(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
  return chm_vat(h, i);
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void chash_i_avx2_u8(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }

  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resize_avx2_u8(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz_avx2_u8(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chash_resize_avx2_u8(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  size_t oc = h->c;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  for (size_t i = 0; i < oc; i++) {
    void *k =s_kat(ods, i, h->sk, h->sv);
    void *v = s_vat(ods, i, h->sk, h->sv);
    if (!(memzero(k, h->sk) &&
          memzero(v, h->sv))) {
      size_t pos = chm_fz_avx2_u8(h, k);
      memcpy(chm_kat(h, pos), k, h->sk);
      memcpy(chm_vat(h, pos), v, h->sv);
      h->n++;
    }
  }
  FREE(ods);
  return nmemb;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void **chash_g_avx2_u8(chash *h, void *k) {
  uint8_t retcode;
  size_t i = chm_fkz_avx2_u8(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
  return chm_vat(h, i);
}
#endif // AVX2

#ifdef __AVX512F__
// TODO: debug remove
void p512_hex_u32(__m512i x, char *prefix) {
  uint32_t v[16] = {0};
  _mm512_storeu_si512((__m512i *)v, x);
  printf("[%s 0] %08x %08x %08x %08x %08x %08x %08x %08x\n", prefix, v[0], v[1],
         v[2], v[3], v[4], v[5], v[6], v[7]);
  printf("[%s 1] %08x %08x %08x %08x %08x %08x %08x %08x\n", prefix, v[8], v[9],
         v[10], v[11], v[12], v[13], v[14], v[15]);
}

static inline size_t chash_resize_avx512f_u4(chash *h, size_t nmemb);
static inline size_t chash_resize_avx512f_u8(chash *h, size_t nmemb);
static inline size_t chm_fkz_avx512f_u4(chash *restrict h, void *restrict k,
                                        uint8_t *restrict ret) {
  size_t i = (h->khash)(k)&h->mod;
  __m512i zeros = _mm512_setzero_si512();
  __m512i vk = _mm512_set1_epi32(*(uint32_t *)k);
avxinsert:
  for (; i < h->c - 8; i += 8) {
    // printf("i = %zu\n", i);
    __m512i a = _mm512_loadu_si512((const __m512i *)chm_kat(h, i));

    // p512_hex_u32(a, "   a");

    uint16_t cmpk = _mm512_cmpeq_epi32_mask(a, vk); // CPI 1
    // printf("cmpk %16b\n", cmpk);
    if (cmpk) {
      // printf("[cmpk] %16b - CTZ=%4d\n", cmpk, __builtin_ctz(cmpk));
      i += (__builtin_ctz(cmpk) >> 1);
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    uint16_t cmpz = _mm512_cmpeq_epi32_mask(a, zeros); // CPI 1
    // printf("cmpz %16b\n", cmpz);
    if (cmpz) {
      // printf("[cmpz] %16b - CTZ=%4d\n", cmpz, __builtin_ctz(cmpz));
      i += (__builtin_ctz(cmpz) >> 1);
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
  }

  for (; i < h->c; i++) {
    if (memcmp(k, chm_kat(h, i), h->sk) == 0) {
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_kat(h, i), h->sv)) {
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
    i = 0;
    goto avxinsert;
  }
out:
  // printf("%zu\n", problen);
  return i;
}

// TODO: check correctness
static inline size_t chm_fkz_avx512f_u8(chash *restrict h, void *restrict k,
                                        uint8_t *restrict ret) {
  size_t i = (h->khash)(k)&h->mod;
  __m512i zeros = _mm512_setzero_si512();
  __m512i vk = _mm512_set1_epi64(*(uint32_t *)k);
avxinsert:
  for (; i < h->c - 4; i += 4) {
    // printf("i = %zu\n", i);
    __m512i a = _mm512_loadu_si512((const __m512i *)chm_kat(h, i));

    // p512_hex_u32(a, "   a");

    uint8_t cmpk = _mm512_cmpeq_epi64_mask(a, vk); // CPI 1
    // printf("cmpk %16b\n", cmpk);
    if (cmpk) {
      // printf("[cmpk] %16b - CTZ=%4d\n", cmpk, __builtin_ctz(cmpk));
      i += (__builtin_ctz(cmpk) >> 1);
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    uint8_t cmpz = _mm512_cmpeq_epi64_mask(a, zeros); // CPI 1
    // printf("cmpz %16b\n", cmpz);
    if (cmpz) {
      // printf("[cmpz] %16b - CTZ=%4d\n", cmpz, __builtin_ctz(cmpz));
      i += (__builtin_ctz(cmpz) >> 1);
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
  }

  for (; i < h->c; i++) {
    if (memcmp(k, chm_kat(h, i), h->sk) == 0) {
      if (ret)
        *ret = CHM_FK;
      goto out;
    }
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_kat(h, i), h->sv)) {
      if (ret)
        *ret = CHM_FZ;
      goto out;
    }
    i = 0;
    goto avxinsert;
  }
out:
  // printf("%zu\n", problen);
  return i;
}

// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void chash_i_avx512f_u4(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }
  // printf("[%s]\n", __func__);
  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resize_avx512f_u4(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz_avx512f_u4(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chash_resize_avx512f_u4(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  size_t oc = h->c;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  // TODO: optimize the insertion
  for (size_t i = 0; i < oc; i++) {
    if (!(memzero(s_kat(ods, i, h->sk, h->sv), h->sk) &&
          memzero(s_vat(ods, i, h->sk, h->sv), h->sv)))
      chash_i_avx512f_u4(h, s_kat(ods, i, h->sk, h->sv),
                         s_vat(ods, i, h->sk, h->sv));
    continue;
  }
  FREE(ods);
  return nmemb;
}

// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void **chash_g_avx512f_u4(chash *h, void *k) {
  uint8_t retcode;
  size_t i = chm_fkz_avx512f_u4(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
  return chm_vat(h, i);
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void chash_i_avx512f_u8(chash *h, void *k, void *value) {
  if (memzero(k, h->sk) && memzero(value, h->sv)) {
    printf("[%s] WARN: key and value set both to zero is not permitted. "
           "This insertion has been ignored.\n",
           __func__);
    return;
  }
  // printf("[%s]\n", __func__);
  if (h->n >= (h->c * CHM_MAXLOAD))
    if (!chash_resize_avx512f_u8(h, h->c << 1))
      exit(EXIT_FAILURE);

  size_t i = chm_fkz_avx512f_u8(h, k, NULL);

  // printf("\tinserted k:%u -> %zu\n", *(uint32_t *)k, i);
  memcpy(chm_kat(h, i), k, h->sk);
  memcpy(chm_vat(h, i), value, h->sv);
  h->n++;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chash_resize_avx512f_u8(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  size_t oc = h->c;
  void *ods = h->ds;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  // TODO: optimize the insertion
  for (size_t i = 0; i < oc; i++) {
    if (!(memzero(s_kat(ods, i, h->sk, h->sv), h->sk) &&
          memzero(s_vat(ods, i, h->sk, h->sv), h->sv)))
      chash_i_avx512f_u4(h, s_kat(ods, i, h->sk, h->sv),
                         s_vat(ods, i, h->sk, h->sv));
    continue;
  }
  FREE(ods);
  return nmemb;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void **chash_g_avx512f_u8(chash *h, void *k) {
  uint8_t retcode;
  size_t i = chm_fkz_avx512f_u8(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
  return chm_vat(h, i);
}
#endif // AVX512F

// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chm_fkz_l(chash *restrict h, void *restrict k,
                               uint8_t *restrict retcode) {
  size_t i = (h->khash)(k)&h->mod;
  while (1) {
    if (memcmp(k, chm_kat(h, i), h->sk) == 0) {
      if (retcode)
        *retcode = CHM_FK;
      return i;
    }
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_vat(h, i), h->sv)) {
      if (retcode)
        *retcode = CHM_FZ;
      return i;
    }
    i = (i + 1) & h->mod;
  }
}
static inline size_t chm_fz_l(chash *restrict h, void *restrict k) {
  size_t i = (h->khash)(k)&h->mod;
  while (1) {
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_vat(h, i), h->sv)) {
      return i;
    }
    i = (i + 1) & h->mod;
  }
}

static inline size_t chm_fkz_peq(chash *restrict h, void *restrict k,
                                 uint8_t *restrict retcode) {
  size_t i = (h->khash)(k)&h->mod;
  while (1) {
    if ((h->keql)(k, chm_kat(h, i))) {
      if (retcode)
        *retcode = CHM_FK;
      return i;
    }
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_vat(h, i), h->sv)) {
      if (retcode)
        *retcode = CHM_FZ;
      return i;
    }
    i = (i + 1) & h->mod;
  }
}
static inline size_t chm_fz_peq(chash *restrict h, void *restrict k) {
  size_t i = (h->khash)(k)&h->mod;
  while (1) {
    if (memzero(chm_kat(h, i), h->sk) && memzero(chm_vat(h, i), h->sv)) {
      return i;
    }
    i = (i + 1) & h->mod;
  }
}

static inline size_t chm_fkz(chash *restrict h, void *restrict k,
                             uint8_t *restrict retcode) {
#ifdef __AVX512F__
  if (h->sv == 4 && h->sk == 4)
    return chm_fkz_avx512f_u4(h, k, retcode);
  if (h->sv == 8 && h->sk == 8)
    return chm_fkz_avx512f_u8(h, k, retcode);
#endif
#ifdef __AVX2__
  if (h->sv == 4 && h->sk == 4)
    return chm_fkz_avx2_u4(h, k, retcode);
  if (h->sv == 8 && h->sk == 8)
    return chm_fkz_avx2_u8(h, k, retcode);
#endif
  return chm_fkz_l(h, k, retcode);
}
static inline size_t chm_fz(chash *restrict h, void *restrict k) {
#ifdef __AVX512F__
  //TODO: fix with fz
  if (h->sv == 4 && h->sk == 4)
    return chm_fkz_avx512f_u4(h, k, retcode);
  if (h->sv == 8 && h->sk == 8)
    return chm_fkz_avx512f_u8(h, k, retcode);
#endif
#ifdef __AVX2__
  if (h->sv == 4 && h->sk == 4)
    return chm_fz_avx2_u4(h, k);
  if (h->sv == 8 && h->sk == 8)
    return chm_fz_avx2_u8(h, k);
#endif
  return chm_fz_l(h, k);
}

// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void **chash_gl(chash *h, void *k) {
  uint8_t retcode;
  size_t i = chm_fkz_l(h, k, &retcode);
  if (retcode == CHM_FZ) {
    return NULL;
  }
  return chm_vat(h, i);
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline void **chash_gA(chash *h, void *k) {
  uint8_t retcode;
  size_t i = (h->fkz)(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
  return chm_vat(h, i);
}
static inline void **chash_g(chash *h, void *k) {
  uint8_t retcode = CHM_FZ;
  size_t i = chm_fkz(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
  return chm_vat(h, i);
}
static inline void **chash_pg(chash *h, void *k) {
  uint8_t retcode = CHM_FZ;
  size_t i = chm_fkz_peq(h, k, &retcode);
  if (retcode == CHM_FZ)
    return NULL;
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

// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chash_resizel(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  size_t oc = h->c;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  for (size_t i = 0; i < oc; i++) {
    void *k = s_kat(ods, i, h->sk, h->sv);
    void *v = s_vat(ods, i, h->sk, h->sv);
    if (!(memzero(k, h->sk) && memzero(v, h->sv))) {
      // chash_pi(h, s_kat(ods, i, h->sk, h->sv), s_vat(ods, i, h->sk, h->sv));
      size_t pos = chm_fz_l(h, k);
      memcpy(chm_kat(h, pos), k, h->sk);
      memcpy(chm_vat(h, pos), v, h->sv);
      h->n++;
    }
  }
  FREE(ods);
  return nmemb;
}
// TODO: this is only for enforcing the same insertion pattern, but should
// not be included in final code
static inline size_t chash_resizeA(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  size_t oc = h->c;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  //TODO: optime insertion
  for (size_t i = 0; i < oc; i++) {
    void *k = s_kat(ods, i, h->sk, h->sv);
    void *v = s_vat(ods, i, h->sk, h->sv);
    if (!(memzero(k, h->sk) && memzero(v, h->sv))) {
      // chash_pi(h, s_kat(ods, i, h->sk, h->sv), s_vat(ods, i, h->sk, h->sv));
      size_t pos = (h->fz)(h, k);
      memcpy(chm_kat(h, pos), k, h->sk);
      memcpy(chm_vat(h, pos), v, h->sv);
      h->n++;
    }
  }
  FREE(ods);
  return nmemb;
}
static inline size_t chash_resize_p(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  size_t oc = h->c;
  h->ds = nds;
  h->n = 0;
  h->mod = nmemb - 1;
  h->c = nmemb;
  for (size_t i = 0; i < oc; i++) {
    void *k = s_kat(ods, i, h->sk, h->sv);
    void *v = s_vat(ods, i, h->sk, h->sv);
    if (!(memzero(k, h->sk) && memzero(v, h->sv))) {
      // chash_pi(h, s_kat(ods, i, h->sk, h->sv), s_vat(ods, i, h->sk, h->sv));
      size_t pos = chm_fz_peq(h, k);
      memcpy(chm_kat(h, pos), k, h->sk);
      memcpy(chm_vat(h, pos), v, h->sv);
      h->n++;
    }
  }
  FREE(ods);
  return nmemb;
}
static inline size_t chash_resize(chash *h, size_t nmemb) {
  // printf("[%s] nmemb = %zu\n", __func__, nmemb);
  if (nmemb < h->n)
    return 0;

  void *nds = calloc(nmemb, h->sk + h->sv);
  if (!nds)
    return 0;

  void *ods = h->ds;
  size_t oc = h->c;
  h->ds = nds;
  h->n = 0;
  h->c = nmemb;
  h->mod = nmemb - 1;
  for (size_t i = 0; i < oc; i++) {
    void *k = s_kat(ods, i, h->sk, h->sv);
    void *v = s_vat(ods, i, h->sk, h->sv);
    if (!(memzero(k, h->sk) && memzero(v, h->sv))) {
      // chash_pi(h, s_kat(ods, i, h->sk, h->sv), s_vat(ods, i, h->sk, h->sv));
      size_t pos = chm_fz_l(h, k);
      memcpy(chm_kat(h, pos), k, h->sk);
      memcpy(chm_vat(h, pos), v, h->sv);
      h->n++;
    }
  }
  FREE(ods);
  return nmemb;
}

#endif
