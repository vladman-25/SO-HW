#include "os_threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* === TASK === */

/* Creates a task that thread must execute */
os_task_t *task_create(void *arg, void (*f)(void *))
{
    // TODO
    os_task_t* newTask = malloc(sizeof(os_task_t));
    if (newTask == NULL) {
        return NULL;
    }
    newTask->argument = arg;
    newTask->task = f;

    return newTask;
}

/* Add a new task to threadpool task queue */
void add_task_in_queue(os_threadpool_t *tp, os_task_t *t)
{
    // TODO
    os_task_queue_t* newTask =  malloc(sizeof(os_task_queue_t));
    if(newTask == NULL) {
        return;
    }

    newTask->task = t;
    newTask->next = NULL;

    if (tp->tasks != NULL) {
        newTask->next = tp->tasks;
    }
    tp->tasks = newTask;
}

/* Get the head of task queue from threadpool */
os_task_t *get_task(os_threadpool_t *tp)
{
    // TODO
    if(tp->tasks == NULL) {
        return NULL;
    }
    os_task_t *task = tp->tasks->task;
    tp->tasks = tp->tasks->next;
    return task;
}

/* === THREAD POOL === */

/* Initialize the new threadpool */
os_threadpool_t *threadpool_create(unsigned int nTasks, unsigned int nThreads)
{
    // TODO
    os_threadpool_t* newPool = malloc(sizeof(os_threadpool_t));
    if (newPool == NULL) {
        return NULL;
    }
    newPool->num_threads = nThreads;
    newPool->should_stop = 0;
    newPool->threads = malloc(nThreads * sizeof(pthread_t));

    newPool->tasks = NULL;
    pthread_mutex_init(&newPool->taskLock, NULL);
    return newPool;
}

/* Loop function for threads */
void *thread_loop_function(void *args)
{
    // TODO
    os_threadpool_t* pool = args;
    while(pool->should_stop == 0) {
        os_task_t* task = NULL;
        while (task == NULL && (pool->should_stop == 0)) {
            pthread_mutex_lock(&pool->taskLock);
            task = get_task(pool);
            pthread_mutex_unlock(&pool->taskLock);
        }
        if (task) {
            void (*f)(void *) = task->task;
            void* arg = task->argument;
            (*f)(arg);
        }
    }
    pthread_exit(NULL);
}

/* Stop the thread pool once a condition is met */
void threadpool_stop(os_threadpool_t *tp, int (*processingIsDone)(os_threadpool_t *))
{
    // TODO
    while(1) {
        if((*processingIsDone)(tp)) {
            tp->should_stop = 1;
            return;
        }
    }
}
