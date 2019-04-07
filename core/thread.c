#include <arch/ia32/reg_context.h>
#include <libc/stdio.h>
#include <thread.h>
#include <core.h>

int thread_init(thread_t *t, int tid, void (*entry_vaddr)(void))
{
    int err;

    // Do *NOT* zero out the main thread's stack while it's using it!
    if (t != &main_thread) {
        memset(t, 0, sizeof(*t));
    }

    t->id = tid;
    t->thread_state = THREAD_STATE_NEW;

    err = reg_context_init(&t->regs, t->stack);
    if (err != 0) {
        printf("Failed to reg_context_init thread.\n");
        return -1;
    }

    reg_context_set_entry_point(&t->regs, entry_vaddr);

    return 0;
}

int thread_init_main_thread(void)
{
    return thread_init(&main_thread, 1, (void (*)(void))&main);
}
