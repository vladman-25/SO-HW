#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 100
#define MAX_THREAD 4

pthread_mutex_t graph_mutex;
pthread_mutex_t sum_mutex;

int sum = 0;
os_graph_t *graph;

struct t_args {
    os_threadpool_t* pool;
    unsigned int node;
};

int t_stop(os_threadpool_t* pool) {
    int ok = 1;

    for(int i=0; i < graph->nCount; i++) {
        ok = ok && graph->visited[i];
    }

    return ok;
}

typedef struct t_args t_args;

void processNode(void * arg)
{
    t_args* args = arg;
    unsigned int nodeIdx = args->node;
    os_threadpool_t* pool = args->pool;

    pthread_mutex_lock(&graph_mutex);
    if (graph->visited[nodeIdx] == 1) {
        pthread_mutex_unlock(&graph_mutex);
        return;
    }
    graph->visited[nodeIdx] = 1;
    pthread_mutex_unlock(&graph_mutex);
    
    os_node_t *node = graph->nodes[nodeIdx];
    pthread_mutex_lock(&sum_mutex);
    sum += node->nodeInfo;
    pthread_mutex_unlock(&sum_mutex);


    for (int i = 0; i < node->cNeighbours; i++) {
        if (graph->visited[node->neighbours[i]] == 0) {
            t_args* new_args = malloc(sizeof(t_args));
            new_args->node = node->neighbours[i];
            new_args->pool = pool;

            add_task_in_queue(pool,task_create(new_args,processNode));
        }
    }
}

void thread_start(os_threadpool_t* pool) {

    for (int i = 0; i < graph->nCount; i++)
    {
        t_args* args = malloc(sizeof(t_args));
        args->node = i;
        args->pool = pool;
        add_task_in_queue(pool,task_create(args,processNode));
    }

    for(int i = 0; i < pool->num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, thread_loop_function, pool);
    }

    threadpool_stop(pool,t_stop);

    for(int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./main input_file\n");
        exit(1);
    }

    FILE *input_file = fopen(argv[1], "r");

    if (input_file == NULL) {
        printf("[Error] Can't open file\n");
        return -1;
    }

    graph = create_graph_from_file(input_file);
    if (graph == NULL) {
        printf("[Error] Can't read the graph from file\n");
        return -1;
    }

    // TODO: create thread pool and traverse the graf
    os_threadpool_t* newPool = threadpool_create(MAX_TASK, MAX_THREAD);
    pthread_mutex_init(&graph_mutex, NULL);
    pthread_mutex_init(&sum_mutex, NULL);
    thread_start(newPool);
    printf("%d", sum);
    return 0;
}
