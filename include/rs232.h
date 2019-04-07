#ifndef _WEK_RS232_H
#define _WEK_RS232_H

#include <stdint.h>

typedef struct rs232_dev_ {
    int id;
    uint16_t port;
} rs232_dev_t;

int8_t rs232_is_initialized(rs232_dev_t *d);
int rs232_initialize(rs232_dev_t *d, int which);
int rs232_puts(rs232_dev_t *d, const char *str);

#endif
