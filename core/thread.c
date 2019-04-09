#include <arch/ia32/reg_context.h>
#include <libc/stdio.h>
#include <thread.h>
#include <core.h>
#include <scheduler.h>


void thread__start(thread_t *self, void (*arg)())
{
    /* The arg passed to the new thread on its stack is its entry point. */
    arg();

    /* We just kill the thread if/when it returns. */
    scheduler_kill_thread(self);
}

int thread_init(thread_t *t, int tid, void (*entry_vaddr)(void))
{
    int err;

    // Do *NOT* zero out the main thread's stack while it's using it!
    if (t != &main_thread) {
        memset(t, 0, sizeof(*t));
    }

    t->id = tid;
    t->thread_state = THREAD_STATE_NEW;

    /* The extra 2 words of space we reserve at the bottom
     * are for setting up the stack of the new thread as if it was called with
     * an argument passed to it on its stack.
     *
     * We need the stack to look like:
     *      | EIP            |
     *      | thread_t *self |
     *      | void *arg      |
     *
     * So that's what the 2 words are for.
     *
     * This only applies to kernel threads and not to user threads.
     */
    t->regs0 = (reg_context_t *)&t->stack[
        WEK_STACK_NBYTES - sizeof(*t->regs0)
        - (sizeof(uintptr_t) * 2)];

    // Do *NOT* edit the main thread's stack while it's using it!
    if (t != &main_thread)
    {
        err = reg_context_init(t->regs0, 0);
        if (err != 0) {
            printf("Failed to reg_context_init regs0 for thread.\n");
            return -1;
        }

        reg_context_set_stack(t->regs0, 0, t->stack);
        reg_context_set_entry_point(t->regs0, 0, (void (*)())&thread__start);
        reg_context_setup_stack_args(t->regs0, t, entry_vaddr);
    }

    err = reg_context_init(&t->regs1, 1);
    if (err != 0) {
        printf("Failed to reg_context_init regs1 for thread.\n");
        return -1;
    }

    reg_context_set_stack(&t->regs1, 1, NULL);
    reg_context_set_entry_point(&t->regs1, 1, entry_vaddr);
    return 0;
}

int thread_init_main_thread(void)
{
    return thread_init(&main_thread, MAIN_THREAD_ID, (void (*)(void))&main);
}

void dump_thread(void *retaddr, reg_context_t **oldctxt, reg_context_t *newctxt)
{
    size_t sz;

    sz = sizeof(reg_context_t);
    printf("Retaddr is %p, oldctxt is %p, newctxt is %p\n"
        "\tsz regs is %d, old+sz is %x, new+sz is %x\n"
        "\tAddress in **old_regs0 is %p, new is %p\n",
        retaddr, oldctxt, newctxt, sz,
        (uintptr_t)oldctxt + sz,
        (uintptr_t)newctxt + sz,
        *oldctxt, newctxt);
}

void dump_ctxt(reg_context_t r)
{
    printf("Stack addr is %p: Eflags %x, ds %x es %x fs %x gs %x,\n"
        "\teax %x, ebx %x, ecx %x, edx %x esi %x, edi %x\n"
        "\tretaddr is %p [off is %d]\n"
        "\tFirst arg is %p [off is %d]; 2nd arg is %p [off is %d]\n",
        &r,
        r.eflags, r.ds, r.es, r.fs, r.gs, r.eax, r.ebx, r.ecx, r.edx,
        r.esi, r.edi,
        r.esp, offsetof(reg_context_t, esp),
        r.ss, offsetof(reg_context_t, ss),
        (&r)[1].es, &(&r)[1]);
}
