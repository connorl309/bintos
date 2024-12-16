#pragma once

#include <stdint.h>
#include <stddef.h>
// Define and implement memmove, memset, memcpy

// Sets `bytes` number of bytes in `dst` to the value specified by `val`.
void* memset(void* dst, const uint8_t val, size_t bytes);
// Copies `bytes` number of bytes from `src` into `dst`.
void* memcpy(void* dst, const void* src, size_t bytes);
// String length
uint32_t strlen(const char* str);
