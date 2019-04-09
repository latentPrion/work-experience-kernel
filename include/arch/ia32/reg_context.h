#ifndef _WEK_REG_CONTEXT_H
#define _WEK_REG_CONTEXT_H

#include <arch/ia32/arch.h>
#include <stdint.h>
#include <libc/string.h>

typedef struct reg_context_ {
	uint32_t		es;		// 0
	uint32_t		ds;		// 4
	uint32_t		gs;		// 8
	uint32_t		fs;		// 12
	uint32_t		edi, esi;	// 16
	uint32_t		ebp, dummyEsp;	// 24
	uint32_t		ebx, edx;	// 32
	uint32_t		ecx, eax;	// 40
	uint32_t		vectorNo;	// 48
	// Error code pushed by the CPU for certain exceptions.
	uint32_t		errorCode;	// 52
	uint32_t		eip, cs;	// 56
	uint32_t		eflags;		// 64
	uint32_t		esp, ss;	// 68
                                // 76
} reg_context_t;

static inline void reg_context_set_entry_point(
    reg_context_t *r, uint8_t regs0_or_regs1, void (*vaddr)(void))
{
    /* For kernel threads this is set on the esp member.
     *
     * For userspace threads it should be set on the usual eip member because
     * userspace returns using iret. Like so:
     *  r->eip = (uintptr_t)vaddr;
     */
    if (regs0_or_regs1 == 0) {
        r->esp = (uintptr_t)vaddr;
    } else {
        r->eip = (uintptr_t)vaddr;
    }
}

static inline void reg_context_set_stack(
    reg_context_t *r, uint8_t regs0_or_regs1, uint8_t *s)
{
    (void)r;
    (void)s;
    /* Nothing to do here for kernel threads.
     * For user threads, we set the "esp" member as usual, like this:
     *  r->esp = (uintptr_t)s;
     */
    if (regs0_or_regs1 == 0) {
    } else {
        r->esp = (uintptr_t)s + WEK_STACK_NBYTES;
    }
}

static inline int reg_context_init(reg_context_t *r, uint8_t regs0_or_regs1)
{
    memset(r, 0, sizeof(*r));

    /* Set GDT segment selectors, etc here. */
    if (regs0_or_regs1 == 0)
    {
        r->cs = 0x08;
        r->ds = r->es = r->fs = r->gs = r->ss = 0x10;
    }
    else
    {
        r->cs = 0x18;
        r->ds = r->es = r->fs = r->gs = r->ss = 0x20;
    }

    // Only set this if the thread should have local #INTs enabled.
    // r->eflags |= x86_EFLAGS_IF;
    return 0;
}

struct thread_;

static inline void reg_context_setup_stack_args(
    reg_context_t *r, struct thread_ *t, void (*arg)())
{
    uintptr_t *stack_arg_location;

    /* We leave r->ss as it is.
     *
     * Note well that r->ss in this situation represents the EIP return address
     * on the stack. But since thread__start should never return, we can
     * neglect to set it.
     *
     * That aside, all we need to do it just set the stack arg.
     */
    stack_arg_location = (uintptr_t *)&r[1];
    stack_arg_location[0] = (uintptr_t)t;
    stack_arg_location[1] = (uintptr_t)arg;
}

void reg_context_save_snapshot_and_switch(
    reg_context_t **old_threads_stack0_current_level,
    reg_context_t *new_threads_stack0_current_level);

#endif
