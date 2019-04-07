#include <multiboot.h>

#define WEK_MULTIBOOT_FLAGS (MULTIBOOT_MEMORY_INFO | MULTIBOOT_PAGE_ALIGN)

/* EXPLANATION:
 * This is a sequence of bytes laid out in a particular order, which the
 * bootloader will search our kernel binary image for. If the bootloader doesn't
 * find this structured sequence of bytes, it will assume that our binary is
 * not a proper kernel image, and refuse to load and execute our binary.
 */
struct multiboot_header
__attribute__((section(".multiboot"), used))
mb_header = {
    /* The bootloader searches for this specific value, the "magic" value */
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = WEK_MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + WEK_MULTIBOOT_FLAGS)
};

multiboot_uint32_t force_keep_multiboot_header(void)
{
    volatile struct multiboot_header *vmbh;

    vmbh = &mb_header;
    return vmbh->flags;
}
