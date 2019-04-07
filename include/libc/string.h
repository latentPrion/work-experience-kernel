#ifndef _WEK_STRING_H
#define _WEK_STRING_H

#include <libc/size_t.h>

void *memset(void *_ptr, int value, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
int memcmp(const void *ptr1, const void *ptr2, size_t n);
void *memmove(void *dest, const void *src, size_t n);
char *strncpy(char *dest, const char *src, size_t count);
int strncmp(const char *str1, const char *str2, int count);
size_t strnlen(const char *str1, size_t maxLen);

#endif
