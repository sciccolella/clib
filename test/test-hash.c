#include "../chash.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t x;
  uint64_t y;
} structest;

// CVEC_DECLAREP(structest *, structestp);

/* #define print_structest_debug(hz) */ \
  /* for (size_t i = 0; i < hz.n; i++) { */ \
    /* printf("%zu: %10d %pt", i, chm_kat(&hz, i), ((structest **)hz.vs)[i]); */ \
    /* if (chmt_vat(&hz, structest *, i) == TOMBSTONE) */ \
      /* printf("T"); */ \
    /* else if (((structest **)hz.vs)[i]) */ \
      /* printf("%zu: %10d {%d, %lu}", i, chm_kat(&hz, i), */ \
             /* ((structest **)hz.vs)[i]->x, ((structest **)hz.vs)[i]->y); */ \
    /* printf("tvat? %d", chm_vat(&hz, i)); */ \
    /* puts(""); */ \
  /* } */

#define print_strstr_debug(hx)                                                 \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10s (h:%2zu) %20p", i, (char *)chm_kat(hx, i),              \
           (hx->khash)(chm_kat(hx, i)) & hx->mod, *(char **)chm_vat(hx, i));   \
    /* printf("t%p", (uint32_t *)chm_vat(hx, i)); */                           \
    char **vi = (char **)chm_vat(hx, i);                                       \
    if (*vi)                                                                   \
      /* printf("-> DEREF"); */                                                \
      printf("-> '%s'", *vi);                                                  \
    puts("");                                                                  \
  };

#define print_str_debug(hx)                                                    \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    char **ki = (char **)chm_kat(hx, i);                                       \
    printf("%3zu: %10s (h:%2zu) %20u", i, *ki ? *ki : "",                      \
           *ki ? (hx->khash)(ki) & hx->mod : 0, *(uint32_t *)chm_vat(hx, i));  \
    printf("\t%p", (uint32_t *)chm_vat(hx, i));                                \
    puts("");                                                                  \
  };

#define print_structest_debug2(hz)                                             \
  for (size_t i = 0; i < (hz)->c; i++) {                                       \
    printf("%3zu: %10d (h:%2zu) %20p", i, *(uint32_t *)chm_kat(hz, i),         \
           (hz->khash)(chm_kat(hz, i)) & hz->mod,                              \
           *(structest **)chm_vat(hz, i));                                     \
    structest **vi = (structest **)chm_vat(hz, i);                             \
    if (*vi)                                                                   \
      printf(" -> {%d, %lu}", (*vi)->x, (*vi)->y);                             \
    puts("");                                                                  \
  }

#define print_int_debug(hx)                                                    \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10d (h:%2zu) %20u", i, *(uint32_t *)chm_kat(hx, i),         \
           (hx->khash)(chm_kat(hx, i)) & hx->mod,                              \
           *(uint32_t *)chm_vat(hx, i));                                       \
    printf("\t%p", (uint32_t *)chm_vat(hx, i));                                \
    puts("");                                                                  \
  };

#define print_int64_debug(hx)                                                  \
  for (size_t i = 0; i < (hx)->c; i++) {                                       \
    printf("%3zu: %10ld (h:%2zu) %20lu", i, *(uint64_t *)chm_kat(hx, i),       \
           (hx->khash)(chm_kat(hx, i)) & hx->mod,                              \
           *(uint64_t *)chm_vat(hx, i));                                       \
    printf("\t%p", (uint64_t *)chm_vat(hx, i));                                \
    /* if (((structest **)hx->vs)[i]) */                                       \
    /* printf(" -> {%d, %lu}", ((structest **)hx->vs)[i]->x, */                \
    /* ((structest **)hx->vs)[i]->y); */                                       \
    puts("");                                                                  \
  };

#define vlit(v)                                                                \
  &(uint32_t) { (v) }
#define vlit64(v)                                                              \
  &(uint64_t) { (v) }
#define slit(s)                                                                \
  &(char *) { (s) }

int main(int argc, char *argv[]) {
#ifdef STRUCTEST
  chash *hz = chash_init(sizeof(uint32_t), sizeof(structest *));
  structest *t1 = malloc(sizeof *t1);
  t1->x = 20;
  t1->y = 20;
  structest *t2 = malloc(sizeof *t2);
  t2->x = 2;
  t2->y = 2;
  structest *t23 = &((structest){23, 23});
  structest *t25 = &((structest){25, 25});
  structest *t27 = &((structest){27, 27});
  structest *t29 = &((structest){29, 29});
  structest *t30 = &((structest){30, 30});
  structest *t31 = &((structest){31, 31});
  structest *t0 = &((structest){0, 0});

  uint32_t k23 = 23;
  printf("hz.s = %zu\n", hz->sv);
  chash_ikl(hz, 20, &t1);
  chash_i(hz, &k23, &t23);
  chash_ikl(hz, 25, &t25);
  // chash_ikl(hz, 2, &t2);
  // chash_ikl(hz, 27, &t27);
  chash_ikl(hz, 29, &t29);
  chash_ikl(hz, 31, &t31);
  chash_ikl(hz, 0, &t0);

  print_structest_debug2(hz);

  // // NOTE: this is a weird and very specific ordering of the keys
  // // to force a circular insertion/deletion
  //
  // chash_i2(hz, 18, &t1);
  // // chasht_i(hz, 21, structest *, t2);
  // chash_i2(hz, 21, &t23);
  // chash_i2(hz, 2, &t2);
  // chash_i2(hz, 5, &t25);
  // chash_i2(hz, 8, &t27);
  // chash_i2(hz, 14, &t29);
  // // chash_i2(hz, 30, &t30);
  // chash_i2(hz, 17, &t31);
  // chash_i2(hz, 20, &t0);
  //
  // chash_d2(hz, 21);
  // chash_i2(hz, 23, &t23);

  // print_structest_debug2(hz);

  // structest *getted = *(structest**)chash_g(hz, 1);
  // structest *gg = *getted;
  structest *getted = chash_gt(hz, vlit(23), structest *);
  printf("getted = %p\n", getted);
  if (getted)
    printf("{%3d, %3lu}\n", getted->x, getted->y);
  else
    puts("NULL");
  puts("DELETE 23");
  chash_d(hz, vlit(23));
  print_structest_debug2(hz);

  // structest *hz_v20 = *(structest **)chash_g(&hz, 20);
  // printf("%d: {%d, %lu}\n", 20, hz_v20->x, hz_v20->y);
  //
  // structest *hz_v20_2 = {0};
  // memcpy(&hz_v20_2, chash_g(&hz, 20), hz.s);
  // printf("%d: {%d, %lu}\n", 20, hz_v20_2->x, hz_v20_2->y);
  //
  // structest *hz_v20t;
  // chasht_g(hz, 20, structest *, hz_v20t);
  // printf("%d: {%d, %lu}\n", 20, hz_v20t->x, hz_v20t->y);

  // chasht_d(hz, 21, structest *);
  puts("DELETE 25");
  chash_d(hz, vlit(25));
  print_structest_debug2(hz);
  puts("INSERT 31");
  chash_ikl(hz, 31, &t31);
  print_structest_debug2(hz);

  printf("h = %p\n", hz);

  size_t done = chash_resize(hz, 1 << 4);
  printf("done = %zu\n", done);
  // printf("h->ks = %p\n", hz->ks);
  printf("h = %p\n", hz);
  print_structest_debug2(hz);

  done = chash_resize(hz, 1 << 3);
  printf("done = %zu\n", done);
  print_structest_debug2(hz);
  // void *tomb = malloc(hz.s);
  // tomb = TOMBSTONE;
  // memcpy(tomb, &TOMBSTONE, hz.s);
  // printf("tomb == TOMBSTONE = %d\n", tomb == &TOMBSTONE);
  // printf("memcmp(tomb, &TOMBSTONE, hz.s) = %d\n", memcmp(tomb, &TOMBSTONE,
  // hz.s)); int check = *(structest **)((void *)&chm_vat(&hz, 0)) == TOMBSTONE;
  // printf("check = %d (%p)\n", check, &chm_vat(&hz, 0));
  // int checkt = ((structest **)(&hz)->vs)[(0)] == TOMBSTONE;
  // printf("checkt = %d (%p)\n", checkt, chmt_vat(&hz, structest *, 0));
  // printf("memcmp((void*)&chm_vat(&hz, 0), &TOMBSTONE, hz.s) = %d\n",
  // memcmp((void*)&chm_vat(&hz, 0), &TOMBSTONE, hz.s));
  // printf("(void*)&chm_vat(&hz, 0) == &TOMBSTONE = %d\n", (void*)&chm_vat(&hz,
  // 0) == &TOMBSTONE); printf("memcmp((void*)&chm_vat(&hz, 2), &TOMBSTONE,
  // hz.s) = %d\n", memcmp((void*)&chm_vat(&hz, 2), &TOMBSTONE, hz.s));
  // printf("(void*)&chm_vat(&hz, 2) == &TOMBSTONE = %d\n", (void*)&chm_vat(&hz,
  // 2) == &TOMBSTONE);

  // chash_i(&hz, 21, &t2);
  // chash_i(&hz, 21, &t2);
  // chash_i(&hz, 24, &t3);
  //
  // // chasht_i(hz, 21, structest *, t2);
  // print_structest_debug(hz);
  // chash_d(&hz, 21);
  // puts("");
  // print_structest_debug(hz);
  //
  // // structest *hz_v24t;
  // structest *hz_v24t = *(structest **)chash_g(&hz, 24);
  // // chasht_g(hz, 25, structest *, hz_v24t);
  // if (hz_v24t)
  //   printf("%d: {%d, %lu}\n", 24, hz_v24t->x, hz_v24t->y);
  // else
  //   printf("NULL\n");
  //
  // chasht_resize(hz, structest *, 20);
  // print_structest_debug(hz);

  free(t1);
  free(t2);
  chash_destroy(hz);
#endif
  puts("------------------------------------------------");
  puts("UINT32");

  chash *h32 = chash_init(sizeof(uint32_t), sizeof(uint32_t));
  chash_i(h32, vlit(21), vlit(21));
  chash_i(h32, vlit(25), vlit(25));
  chash_i(h32, vlit(2), vlit(2));
  chash_i(h32, vlit(27), vlit(27));
  // NOTE: this will be ignored. Both key and value of 0 is not permitted.
  chash_i(h32, vlit(0), vlit(0));
  chash_i(h32, vlit(3), vlit(3));
  print_int_debug(h32);

  uint32_t *got32;
  got32 = (uint32_t *)chash_g(h32, vlit(25));
  printf("got (25) = %p -> ", got32);
  if (got32)
    printf("%u\n", *got32);
  else
    puts("NULL");
  got32 = (uint32_t *)chash_g(h32, vlit(26));
  printf("got (26) = %p -> ", got32);
  if (got32)
    printf("%u\n", *got32);
  else
    puts("NULL");
  puts("DELETE 25");
  chash_d(h32, vlit(25));
  puts("REPLACE VAL(3): 3->25");
  chash_i(h32, vlit(3), vlit(25));
  print_int_debug(h32);

  puts("------------------------------------------------");
  puts("UINT64");

  chash *h64 = chash_init(sizeof(uint64_t), sizeof(uint64_t));
  chash_i(h64, vlit64(21), vlit64(21));
  chash_i(h64, vlit64(25), vlit64(25));
  chash_i(h64, vlit64(2), vlit64(2));
  chash_i(h64, vlit64(27), vlit64(27));
  // NOTE: this will be ignored. Both key and value of 0 is not permitted.
  chash_i(h64, vlit64(0), vlit64(0));
  chash_i(h64, vlit64(3), vlit64(3));
  print_int64_debug(h64);

  uint64_t *got64;
  got64 = (uint64_t *)chash_g(h64, vlit64(25));
  printf("got (25) = %p -> ", got64);
  if (got64)
    printf("%lu\n", *got64);
  else
    puts("NULL");
  got64 = (uint64_t *)chash_g(h64, vlit64(26));
  printf("got (26) = %p -> ", got64);
  if (got64)
    printf("%lu\n", *got64);
  else
    puts("NULL");
  puts("DELETE 25");
  chash_d(h64, vlit64(25));
  puts("REPLACE VAL(3): 3->25");
  chash_i(h64, vlit64(3), vlit64(25));
  print_int64_debug(h64);

  puts("------------------------------------------------");
  puts("STRUCTEST");

  chash *hst = chash_init(sizeof(uint32_t), sizeof(structest *));
  structest *t21 = malloc(sizeof *t21);
  t21->x = 21;
  t21->y = 21;
  structest *t2 = malloc(sizeof *t2);
  t2->x = 2;
  t2->y = 2;
  structest *t25 = &((structest){25, 25});
  structest *t27 = &((structest){27, 27});
  structest *t3 = &((structest){3, 3});
  structest *t0 = &((structest){0, 0});

  chash_i(hst, vlit(21), &t21);
  chash_i(hst, vlit(25), &t25);
  chash_i(hst, vlit(2), &t2);
  chash_i(hst, vlit(27), &t27);
  chash_i(hst, vlit(0), &t0);
  chash_i(hst, vlit(3), &t3);
  print_structest_debug2(hst);

  structest **gotst;
  gotst = (structest **)chash_g(hst, vlit(25));
  printf("got (25) = %p -> ", gotst);
  if (gotst)
    printf("{%3d, %3lu}\n", (*gotst)->x, (*gotst)->y);
  else
    puts("NULL");
  gotst = (structest **)chash_g(hst, vlit(26));
  printf("got (26) = %p -> ", gotst);
  if (gotst)
    printf("{%3d, %3lu}\n", (*gotst)->x, (*gotst)->y);
  else
    puts("NULL");

  chash_d(hst, vlit(25));
  puts("REPLACE VAL(3): 3->25");
  chash_i(hst, vlit(3), &t25);
  print_structest_debug2(hst);

  puts("------------------------------------------------");
  puts("STRING");
  chash *hstr = chash_init(sizeof(char *), sizeof(uint32_t));
  hstr->khash = hash_str;
  hstr->keql = keql_str;

  char *s21 = malloc(25 * sizeof *s21);
  strcat(s21, "tentyonepilots");
  // printf("s21 = %p\n", s21);
  // printf("&s21 = %p\n", &s21);
  chash_pi(hstr, &s21, vlit(21));
  chash_pi(hstr, slit("s25"), vlit(25));
  chash_pi(hstr, slit("two"), vlit(2));
  chash_pi(hstr, slit("twentyseven"), vlit(27));
  chash_pi(hstr, slit("zero"), vlit(0));
  chash_pi(hstr, slit("three"), vlit(3));
  chash_pi(hstr, slit("fucking hellss"), vlit(666));
  print_str_debug(hstr);

  uint32_t *gotstr32;
  gotstr32 = (uint32_t *)chash_pg(hstr, slit("s25"));
  printf("got (\"s25\") = %p -> ", gotstr32);
  if (gotstr32)
    printf("%u\n", *gotstr32);
  else
    puts("NULL");
  gotstr32 = (uint32_t *)chash_pg(hstr, slit("twentysev"));
  printf("got (\"twentysev\") = %p -> ", gotstr32);
  if (gotstr32)
    printf("%u\n", *gotstr32);
  else
    puts("NULL");

  puts("DELETE \"zero\"");
  chash_pd(hstr, slit("zero"));
  puts("REPLACE VAL(\"three\"): 3->25");
  chash_pi(hstr, slit("three"), vlit(25));
  print_str_debug(hstr);

  // puts("------------------------------------------------");
  // puts("STRING-STRING");
  // chash *hstrstr = chash_init(sizeof(char *), sizeof(char *));
  // hstrstr->khash = fnv1a_hash32;
  // hstrstr->keql = keql_str;
  //
  // char x[] = "fuck";
  // char (*p)[] = &x;
  // printf("x = %p\n", x);
  // printf("&x = %p\n", &x);
  // printf("p = %p\n", p);
  // printf("&p = %p\n", &p);
  // chash_pi(hstrstr, "s21", &p);
  // chash_pi(hstrstr, &s25, slit("twentyfive"));
  // chash_pi(hstrstr, "two", slit("s2"));
  // chash_pi(hstrstr, "27", slit("twentyseven"));
  // chash_pi(hstrstr, "zero", slit("000"));
  // chash_pi(hstrstr, "three", slit("3"));
  // print_strstr_debug(hstrstr);

  // uint32_t *gotstr32;
  // gotstr32 = (uint32_t *)chash_pg(hstr, "s25");
  // printf("got (\"s25\") = %p -> ", gotstr32);
  // if (gotstr32)
  //   printf("%u\n", *gotstr32);
  // else
  //   puts("NULL");
  // gotstr32 = (uint32_t *)chash_g(hstr, "twentysix");
  // printf("got (\"twentysix\") = %p -> ", gotstr32);
  // if (gotstr32)
  //   printf("%u\n", *gotstr32);
  // else
  //   puts("NULL");
  //
  // puts("DELETE \"zero\"");
  // chash_pd(hstr, "zero");
  // puts("REPLACE VAL(\"three\"): 3->25");
  // chash_pi(hstr, "three", vlit(25));
  // print_str_debug(hstr);

  return EXIT_SUCCESS;
}
