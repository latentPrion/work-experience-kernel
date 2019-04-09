#ifndef _WEK_THREAD_H
#define _WEK_THREAD_H

#include <stdint.h>
#include <arch/ia32/arch.h>
#include <arch/ia32/reg_context.h>
#include <linked_list.h>
#include <thread_state.h>

typedef struct thread_ {
    /* This is the ring0 stack. */
    uint8_t stack[WEK_STACK_NBYTES];
    list_element_t scheduler_list;
    enum thread_state thread_state;
    int id;
    /* This reg_context_t is only used for userspace threads.
     * Kernel threads save and restore their reg context on their stacks.
     */
    reg_context_t regs1;
    /* Per the above comment, this is a pointer to an offset within the stack of
     * the thread.
     */
    reg_context_t *regs0;
} thread_t;

#define MAIN_THREAD_ID (1)
#define IDLE_THREAD_ID (2)

extern thread_t idle_thread, main_thread;
extern thread_t fleas1, fleas2, fleas3, fleas4;

int thread_init(thread_t *t, int tid, void (*entry_vaddr)(void));
int thread_init_idle_thread(void);
int thread_init_main_thread(void);
int all_fleas_init(void);

#endif
