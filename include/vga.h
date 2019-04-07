#ifndef _WEK_VGA_H
#define _WEK_VGA_H

#include <stdint.h>
#include <libc/size_t.h>

typedef struct vga_char_ {
    uint8_t ch, colour;
} vga_char_t;

typedef struct vga_alphanum_dev_ {
    vga_char_t *fb;
    uint8_t row, col;
} vga_alphanum_dev_t;

int vga_alphanum_init(vga_alphanum_dev_t *d);
int8_t vga_alphanum_is_initialized(vga_alphanum_dev_t *d);
void vga_alphanum_clear(vga_alphanum_dev_t *d);
size_t vga_alphanum_puts(vga_alphanum_dev_t *d, const char *str);
size_t vga_alphanum_putc(vga_alphanum_dev_t *d, const char c);
size_t vga_alphanum_putc_at(
    vga_alphanum_dev_t *d, uint8_t row, uint8_t col, char c);

#endif
