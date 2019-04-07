#ifndef _WEK_RS232_H
#define _WEK_RS232_H

#include <stdint.h>

typedef struct rs232_dev_ {
    int id;
    uint16_t port;
} rs232_dev_t;

#endif
