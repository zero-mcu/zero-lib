#include <common/zero.h>


static slist_t co_thread_list = SLIST_INIT(co_thread_list);

void co_thread_add(co_thread_t* pthread, void (*entry)(void* arg), void* arg)
{
    ASSERT(pthread);
    ASSERT(entry);

    pthread->entry = entry;
    pthread->arg = arg;
    pthread->flag = CO_THREAD_IDLE;
    slist_append(&co_thread_list, &pthread->node);
}

void co_thread_remove(co_thread_t* pthread)
{
    ASSERT(pthread);

    slist_remove(&co_thread_list, &pthread->node);
}

void co_thread_start(co_thread_t* pthread)
{
    ASSERT(pthread);

    pthread->flag = CO_THREAD_RUNNING;
}

void co_thread_stop(co_thread_t* pthread)
{
    ASSERT(pthread);

    pthread->flag = CO_THREAD_PENDING;
}

void co_thread_schedule(void)
{
    slist_t* pos = NULL;

    slist_for_each(pos, &co_thread_list)
    {
        co_thread_t* thread = slist_entry(pos, co_thread_t, node);
        if (thread->flag == CO_THREAD_RUNNING)
            thread->entry(thread->arg);
    }
}

