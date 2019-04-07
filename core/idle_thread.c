
#include <scheduler.h>
#include <libc/stdio.h>
#include <thread.h>

#define IDLE_THREAD_ID (2)

thread_t idle_thread;

static void idle_thread_main(void)
{
    printf("Entering idle thread!\n");

    for (;;) {
//        asm volatile ("hlt\n\t");
        scheduler_yield();
    }
}

int thread_init_idle_thread(void)
{
    return thread_init(&idle_thread, IDLE_THREAD_ID, idle_thread_main);
}
