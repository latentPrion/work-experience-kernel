#ifndef _WEK_CORE_H
#define _WEK_CORE_H

#include <libc/coretypes.h>
#include <multiboot.h>

#define WEK_VADDR_BASE (0xC0000000)
#define WEK_PADDR_LOAD (0x100000)

#ifndef __ASSEMBLER__
__attribute__((noreturn)) void panic(const char *fmt, ...);
int main(uint32_t mb_magic, multiboot_info_t *mb_info);
#endif

#endif
