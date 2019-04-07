#include <stdint.h>
#include <libc/size_t.h>
#include <libc/string.h>
#include <vga.h>
#include <core.h>

#define VGA_ALPHANUM_PADDR      (0xb8000)
#define VGA_ALPHANUM_NROWS      (25)
#define VGA_ALPHANUM_NCOLS      (80)

#define VGA_ALPHANUM_GREY       (0x07)

/** EXPLANATION:
 * This is a very basic VGA driver which relies on the fact that most desktop
 * and laptop computers will boot up with their graphics card in a "text" or
 * "alphanumeric" mode instead of the rasterized framebuffer mode for drawing
 * images.
 *
 * We've written some basic routines in here to take any text you try to print,
 * and print them onto the screen, with scrolling included.
 */

typedef struct vga_alphanum_coords_ {
    uint8_t row, col;
} vga_alphanum_coords_t;

int vga_alphanum_init(vga_alphanum_dev_t *d)
{
    if (d == NULL) { return -1; }

    d->row = d->col = 0;
    d->fb = (vga_char_t *)VGA_ALPHANUM_PADDR;
    return 0;
}

int8_t vga_alphanum_is_initialized(vga_alphanum_dev_t *d)
{
    return d->fb == (vga_char_t *)VGA_ALPHANUM_PADDR;
}

static int8_t vga_alphanum_sanitize_coords(uint8_t *row, uint8_t *col)
{
    int8_t requires_scrollup = 0;

    if (*col >= VGA_ALPHANUM_NCOLS)
    {
        *col = 0;
        *row += 1;
    }

    if (*row >= VGA_ALPHANUM_NROWS)
    {
        requires_scrollup = 1;
        *row = VGA_ALPHANUM_NROWS - 1;
    }

    return requires_scrollup;
}

static void vga_alphanum_scrollup(vga_alphanum_dev_t *d)
{
    memmove(
        d->fb, &d->fb[VGA_ALPHANUM_NCOLS],
        sizeof(*d->fb) * (VGA_ALPHANUM_NROWS - 1) * VGA_ALPHANUM_NCOLS);

    memset(&d->fb[(VGA_ALPHANUM_NROWS - 1) * VGA_ALPHANUM_NCOLS], 0,
        sizeof(*d->fb) * VGA_ALPHANUM_NCOLS);
}

size_t vga_alphanum_putc_at(
    vga_alphanum_dev_t *d, uint8_t row, uint8_t col, char c)
{
    vga_char_t *vc;

    /** EXPLANATION:
     * Unintelligent putc_at(coords).
     *
     * If you give it coords beyond the framebuffer size it'll just do nothing
     * and return. No coord clipping is done.
     *
     * @return The number of chars printed.
     */

    if (d == NULL) { return 0; }
    if (col >= VGA_ALPHANUM_NCOLS || row >= VGA_ALPHANUM_NROWS) { return 0; }

    vc = &d->fb[row * VGA_ALPHANUM_NCOLS + col];
    vc->ch = c;
    vc->colour = VGA_ALPHANUM_GREY;
    return 1;
}

__attribute__((unused)) static size_t vga_alphanum_puts_at(
    vga_alphanum_dev_t *d, uint8_t row, uint8_t col, const char *s,
    vga_alphanum_coords_t *next_coords)
{
    (void)vga_alphanum_puts_at;
    (void)row;
    (void)col;
    (void)next_coords;

    /** EXPLANATION:
     * Puts directly to the framebuffer at coords specified.
     *
     * Will advance the printing by row, but will not scroll the device's
     * framebuffer contents if you exceed the framebuffer limits.
     *
     * Will simply return when it hits the framebuffer limits if you reach them.
     *
     * @return Returns the number of characters printed.
     * @param[out] next_coords If return value is greater than 0, then this is
     *                    the coords in the buffer of the next position
     *                    to print to right after this function has finished
     *                    printing.
     */

    if (d == NULL || s == NULL) {
        return 0;
    }

    // UNIMPLEMENTED.
    return 0;
}

size_t vga_alphanum_putc(vga_alphanum_dev_t *d, const char c)
{
    size_t nprinted;

    switch (c)
    {
    case '\r':
        d->col = 0;
        break;

    case '\n':
        d->col = 0;
        d->row++;
        break;

    case '\t':
        // Indent by 8 spaces.
        d->col >>= 3;
        d->col += 1;
        d->col <<= 3;
        break;

    default:
        nprinted = vga_alphanum_putc_at(d, d->row, d->col, c);
        if (nprinted == 0) {
            return 0;
        }
        d->col++;
    }

    if (vga_alphanum_sanitize_coords(&d->row, &d->col)) {
        vga_alphanum_scrollup(d);
    }

    return nprinted;
}

size_t vga_alphanum_puts(vga_alphanum_dev_t *d, const char *str)
{
    int i;
    size_t nprinted;

    if (d == NULL || str == NULL) { return 0; }

    for (i=0; str[i] != '\0'; i++) {
        nprinted = vga_alphanum_putc(d, str[i]);
        if (nprinted < 1) {
            panic("Unknown error! nprinted was 0.\n");
        }
    }

    return i;
}

void vga_alphanum_clear(vga_alphanum_dev_t *d)
{
    memset(
        d->fb, 0,
        sizeof(*d->fb) * VGA_ALPHANUM_NCOLS * VGA_ALPHANUM_NROWS);
}
