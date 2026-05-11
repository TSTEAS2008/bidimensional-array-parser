#ifndef BDA_H
#define BDA_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint64_t n;
    uint64_t x;
    uint64_t y;
    uint64_t src;
    uint64_t dest;
} bda_descriptor;

void bda_parse(bda_descriptor *bda);

#endif