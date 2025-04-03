#pragma once
#include <stdint.h>
#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);

void *memset(void *s, int c, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);
// Halt and catch fire function.
void hcf(void);