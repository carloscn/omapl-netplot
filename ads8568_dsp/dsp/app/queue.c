#include <stdlib.h>
#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include "queue.h"
#include "log.h"

struct _Queue {
    Queue_Handle handle; 
    Semaphore_Handle sem;
};

Queue *queue_new() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    memset(queue, 0, sizeof(Queue));

    queue->handle = Queue_create(NULL, NULL);
    if (! queue->handle)
        LOG_FATAL("failed to create queue");

    /* 信号量。用于同步 */
    queue->sem = Semaphore_create(0, NULL, NULL);
    if (! queue->sem)
        LOG_FATAL("failed to create semaphore");

    return queue;
}

void queue_destroy(Queue *queue) {
    if (! queue)
        return;

    Queue_delete(&queue->handle);
    Semaphore_delete(&queue->sem);
    free(queue);
}

void queue_push(Queue *queue, Queue_Elem *elm) {
    ASSERT(queue);
    
    Queue_put(queue->handle, elm);  
    Semaphore_post((Semaphore_Object *)queue->sem); /* notity */
}

Queue_Elem *queue_pop(Queue *queue) {
    ASSERT(queue);
    
    /* 等待 notify，进行数据读取 */
    while (Queue_empty(queue->handle))
        Semaphore_pend(queue->sem, BIOS_WAIT_FOREVER);

    return Queue_get(queue->handle);
}
