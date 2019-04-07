#ifndef _WEK_SCHEDULER_H
#define _WEK_SCHEDULER_H

#include <linked_list.h>
#include <thread.h>

typedef struct scheduler_ {
    int total_n_threads;
    int n_runnable;
    int n_blocked;
    int n_sleeping;

    thread_t *current_thread;

    linked_list_t runqueue;
} scheduler_t;

extern scheduler_t scheduler;

int scheduler_init(void);
int scheduler_add_thread(thread_t *t);
int scheduler_wake_thread(thread_t *t);
void scheduler_sleep_thread(thread_t *t);
void scheduler_block_thread(thread_t *t);
__attribute__((noreturn))
void scheduler_kill_thread(thread_t *t);
int scheduler_yield(void);

void set_current_thread(thread_t *t);
thread_t *get_current_thread(void);


#endif
