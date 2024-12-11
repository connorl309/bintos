#include "stdlib.h"

// Sets `bytes` number of bytes in `dst` to the value specified by `val`.
void* memset(void* dst, const uint8_t val, size_t bytes) {
    
    for (size_t i = 0; i < bytes; i++)
        ((uint8_t*)dst)[i] = val;
    return dst;
}
// Copies `bytes` number of bytes from `src` into `dst`.
void* memcpy(void* dst, const void* src, size_t bytes) {
    if (dst == src) return src;
    for (size_t i = 0; i < bytes; i++)
        ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
    return dst;
}
