#ifndef _WEK_ARCH_H
#define _WEK_ARCH_H

#define PAGE_SIZE (0x1000)
#define WEK_STACK_NBYTES (PAGE_SIZE * 4)

#define x86_EFLAGS_IF       (1<<9)

#ifndef __ASSEMBLER__
int arch_init(void);
#endif

#endif
