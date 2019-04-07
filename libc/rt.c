#include <stdint.h>
#include <stddef.h>
#include <libc/string.h>

extern uint8_t __k_bss_start, __k_bss_end;

void __wek_libc_pre_main(void)
{
    ptrdiff_t bss_nbytes;

    bss_nbytes = &__k_bss_end - &__k_bss_start;
    memset(&__k_bss_start, 0, bss_nbytes);
}

void __wek_libc_post_main(void)
{
}
