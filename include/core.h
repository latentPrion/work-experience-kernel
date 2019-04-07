#ifndef _WEK_CORE_H
#define _WEK_CORE_H

#include <arch/ia32/arch.h>
#include <libc/coretypes.h>

#define WEK_STACK_NBYTES (PAGE_SIZE * 4)
#define WEK_VADDR_BASE (0xC0000000)
#define WEK_PADDR_LOAD (0x100000)

#ifndef __ASSEMBLER__
__attribute__((noreturn)) void panic(const char *fmt, ...);
#endif

#endif
