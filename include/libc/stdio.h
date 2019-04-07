#ifndef _WEK_STDIO_H
#define _WEK_STDIO_H

#include <stdint.h>
#include <stdarg.h>
#include <libc/coretypes.h>
#include <libc/size_t.h>

int printf_init(void);
int8_t printf_is_initialized(void);

int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int snprintf(char *buff, size_t maxlen, const char *fmt, ...);
int vsnprintf(char *buff, size_t maxlen, const char *fmt, va_list args);

#endif
