#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/syslink/utils/IHeap.h>

#include "message_heap.h"
#include "log.h"

struct _MessageHeap {
    IHeap_Handle heap;
    unsigned int id;
};

MessageHeap * message_heap_new(unsigned int shared_region_id) {
    MessageHeap *heap = (MessageHeap *)malloc(sizeof(MessageHeap));
    memset(heap, 0, sizeof(MessageHeap));

    heap->heap = (IHeap_Handle)SharedRegion_getHeap(shared_region_id);
    heap->id = 1;

    if (heap->heap == NULL)
        LOG_FATAL("failed to get heap in shareregion %d\n", shared_region_id);

    /* 堆绑定 */
    if (MessageQ_registerHeap((Ptr)(heap->heap), heap->id) < 0)
        LOG_FATAL("failed to register heap\n");

    return heap;
}

void message_heap_destroy(MessageHeap *heap) {
    if (! heap)
        return;

    /* 解绑定 */
    MessageQ_unregisterHeap(heap->id);
    free(heap);
}

unsigned int message_heap_id(MessageHeap *heap) {
    ASSERT(heap); 
    return heap->id;
}
