#ifndef _WEK_THREAD_H
#define _WEK_THREAD_H

#include <stdint.h>
#include <arch/ia32/arch.h>
#include <arch/ia32/reg_context.h>
#include <linked_list.h>
#include <thread_state.h>

typedef struct thread_ {
    uint8_t stack[WEK_STACK_NBYTES];
    list_element_t scheduler_list;
    enum thread_state thread_state;
    int id;
    reg_context_t regs;
} thread_t;

extern thread_t idle_thread, main_thread;

int thread_init(thread_t *t, int tid, void (*entry_vaddr)(void));
int thread_init_idle_thread(void);
int thread_init_main_thread(void);

#endif
