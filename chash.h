#ifndef CHASH_H
#define CHASH_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t n;
  void *data;
} chash;

[[nodiscard]] static inline chash chash_init(size_t size, size_t nmemb) {
  return (chash){.n = nmemb, .data = malloc(size * nmemb)};
}

#endif
