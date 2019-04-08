
#include <scheduler.h>
#include <libc/stdio.h>
#include <thread.h>

thread_t idle_thread;

static void idle_thread_main(void)
{
    printf("Entering idle thread!\n");

    for (;;) {
        asm volatile("pause\n");
        scheduler_yield();
    }
}

int thread_init_idle_thread(void)
{
    return thread_init(&idle_thread, IDLE_THREAD_ID, idle_thread_main);
}
