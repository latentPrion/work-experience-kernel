#include <libc/string.h>
#include <libc/stdio.h>
#include <core.h>
#include <scheduler.h>
#include <linked_list.h>
#include <thread.h>
#include <thread_state.h>

scheduler_t scheduler;

void set_current_thread(thread_t *t)
{
    scheduler.current_thread = t;
}

thread_t *get_current_thread(void)
{
    if (scheduler.current_thread == NULL) {
        panic("Current_thread is NULL!\n");
    }

    return scheduler.current_thread;
}

int scheduler_init(void)
{
    int err;

    memset(&scheduler, 0, sizeof(scheduler));

    // Init the run queue linked list.
    err = list_init(&scheduler.runqueue);
    if (err != 0) {
        return err;
    }

    set_current_thread(&main_thread);
    return 0;
}

int scheduler_add_thread(thread_t *t)
{
    if (t->thread_state != THREAD_STATE_NEW) {
        panic("add_thread: Attempt to add thread which is not new.\n");
    }

    return scheduler_wake_thread(t);
}

int scheduler_wake_thread(thread_t *t)
{
    if (t->thread_state == THREAD_STATE_RUNNABLE) {
        panic("wake_thread: Attempt to wake runnable thread!\n");
    }

    if (!(t->thread_state == THREAD_STATE_NEW && t == &main_thread))
    {
        /* If the main thread is being newly added to the runqueue at boot, we
         * can't actually add it to the runqueue since it's the currently
         * executing thread -- and the currently executing thread shouldn't be
         * in the runqueue.
         */
        list_append(&scheduler.runqueue, &t->scheduler_list);
    }

    if (t->thread_state == THREAD_STATE_BLOCKED) {
        scheduler.n_blocked--;
    } else if (t->thread_state == THREAD_STATE_SLEEPING) {
        scheduler.n_sleeping--;
    } else if (t->thread_state == THREAD_STATE_NEW) {
        scheduler.total_n_threads++;
    }
    else
    {
        panic("wake_thread: Thread was in unknown state %d!\n",
            t->thread_state);
    }

    t->thread_state = THREAD_STATE_RUNNABLE;
    scheduler.n_runnable++;
    return 0;
}

static int scheduler_remove_thread_from_runqueue(
    thread_t *t, enum thread_state new_state)
{
    int err;

    if (t->thread_state != THREAD_STATE_RUNNABLE) {
        panic("sleep_thread: Attempt to sleep non-runnable thread.\n");
    }

    /* It is fine for a thread either to be or not to be in the runqueue in this
     * case.
     *
     * A thread that is sleeping itself will not find itself in the run queue.
     * A thread that is sleeping another thread should find that other thread in
     * the run queue unless this is a double-call to sleep for that thread.
     */
    err = list_remove(&scheduler.runqueue, &t->scheduler_list);
    if (t == get_current_thread() && err == 0)
    {
        panic("remove_thread_from_runqueue: Found ourself in the runqueue "
            "when trying to remove ourself.\n"
            "Since this is a uniprocessor kernel, the current thread should "
            "never be in the runqueue\n");
    }

    scheduler.n_runnable--;

    t->thread_state = new_state;
    if (new_state == THREAD_STATE_BLOCKED) {
        scheduler.n_blocked++;
    } else if (new_state == THREAD_STATE_SLEEPING) {
        scheduler.n_sleeping++;
    } else if (new_state == THREAD_STATE_NEW) {
        scheduler.total_n_threads--;
    }

    return 0;
}

static thread_t *scheduler_get_next_thread(void)
{
    list_element_t *t_elem;
    thread_t *ret;
    uintptr_t off;

    t_elem = list_pop_head(&scheduler.runqueue);
    off = offsetof(thread_t, scheduler_list);
    ret = (thread_t *)((uintptr_t)t_elem - off);
    return ret;
}

static void scheduler_switch_to_next_thread(void)
{
    thread_t *next, *curr;

    next = scheduler_get_next_thread();
    if (next == NULL) {
        panic("pull: get_next_thread() returned NULL!\n");
    }

    if (list_remove(&scheduler.runqueue, &next->scheduler_list) == 0) {
        panic("pull: just popped a thread from runqueue, but found it in the "
            "queue again!\n");
    }

    curr = get_current_thread();
    if (next != curr)
    {
        /* Can't call set_current_thread() after the save_snap_and_switch()
         * because then the set_current_thread() line won't be executed until
         * after the thread that gets suspended is eventually rescheduled -- and
         * that would mean that in the interim, the scheduler's metadata for
         * the current thread is not up to date.
         */
        set_current_thread(next);
        // This line below is the exact moment when the threads are switched.
        reg_context_save_snapshot_and_switch(
            &curr->regs0, next->regs0);
    }
}

__attribute__((unused))
static void schedule(void)
{
    scheduler_switch_to_next_thread();
}

void scheduler_sleep_thread(thread_t *t)
{
    int err;

    err = scheduler_remove_thread_from_runqueue(t, THREAD_STATE_SLEEPING);
    if (err != 0) {
        panic("sleep_thread: failed to remove thread from runqueue!\n");
    }
    scheduler_switch_to_next_thread();
}

void scheduler_block_thread(thread_t *t)
{
    int err;

    err = scheduler_remove_thread_from_runqueue(t, THREAD_STATE_BLOCKED);
    if (err != 0) {
        panic("block_thread: failed to remove thread from runqueue!\n");
    }

    scheduler_switch_to_next_thread();
}

__attribute__((noreturn))
void scheduler_kill_thread(thread_t *t)
{
    int err;

    err = scheduler_remove_thread_from_runqueue(t, THREAD_STATE_NEW);
    if (err != 0) {
        panic("kill_thread: failed to remove thread from runqueue!\n");
    }
    scheduler_switch_to_next_thread();
    __builtin_unreachable();
}

static int scheduler_yield_thread(thread_t *t)
{
    int err;

    if (t->thread_state != THREAD_STATE_RUNNABLE) {
        panic("yield: Attempt to yield thread that is not runnable.\n");
    }

    err = list_remove(&scheduler.runqueue, &t->scheduler_list);
    if (err == 0) {
        panic("yield: Thread which is yielding was already in the runqueue!\n");
    }

    list_append(&scheduler.runqueue, &t->scheduler_list);
    scheduler_switch_to_next_thread();
    return 0;
}

int scheduler_yield(void)
{
    return scheduler_yield_thread(get_current_thread());
}
