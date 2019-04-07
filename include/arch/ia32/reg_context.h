#ifndef _WEK_REG_CONTEXT_H
#define _WEK_REG_CONTEXT_H

#include <arch/ia32/arch.h>
#include <stdint.h>
#include <libc/string.h>

typedef struct reg_context_ {
	uint16_t		es;		// 0
	uint16_t		ds;		// 2
	uint16_t		gs;		// 4
	uint16_t		fs;		// 6
	uint32_t		edi, esi;	// 8
	uint32_t		ebp, dummyEsp;	// 16
	uint32_t		ebx, edx;	// 24
	uint32_t		ecx, eax;	// 32
	uint32_t		vectorNo;	// 40
	// Error code pushed by the CPU for certain exceptions.
	uint32_t		errorCode;	// 44
	uint32_t		eip, cs;	// 48
	uint32_t		eflags;		// 56
	uint32_t		esp, ss;	// 60
} reg_context_t;

static inline void reg_context_set_entry_point(
    reg_context_t *r, void (*vaddr)(void))
{
    r->eip = (uintptr_t)vaddr;
}

static inline void reg_context_set_stack(reg_context_t *r, uint8_t *s)
{
    r->esp = (uintptr_t)&s[WEK_STACK_NBYTES];
}

static inline int reg_context_init(reg_context_t *r, uint8_t *stack)
{
    memset(r, 0, sizeof(*r));
    reg_context_set_stack(r, stack);

    /* Set GDT segment selectors, etc here. */
    r->cs = 0x08;
    r->ds = r->es = r->fs = r->gs = r->ss = 0x10;

    // Only set this if the thread should have local #INTs enabled.
    // r->eflags |= x86_EFLAGS_IF;
    return 0;
}

void reg_context_save_snapshot_and_switch(
    reg_context_t *oldc, reg_context_t *newc);

#endif
