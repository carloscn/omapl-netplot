#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ti/syslink/Std.h>     /* must be first */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/syslink/utils/IHeap.h>

#include "message_heap.h"
#include "log.h"

struct _MessageHeap {
    IHeap_Handle heap;
    uint32_t id;
};

MessageHeap * message_heap_new(uint32_t shared_region_id) {
    MessageHeap *heap = (MessageHeap *)malloc(sizeof(MessageHeap));
    memset(heap, 0, sizeof(MessageHeap));

    heap->heap = (IHeap_Handle)SharedRegion_getHeap(shared_region_id);
    heap->id = 1;

    if (heap->heap == NULL)
        LOG_INFO("heap is null\n");

    /* 绑定堆 */
    if (MessageQ_registerHeap((Ptr)(heap->heap), heap->id) < 0) {
        LOG_INFO("failed to register heap\n");
    }

    return heap;
}

void message_heap_destroy(MessageHeap *heap) {
    if (! heap)
        return;

    /* 解绑定 */
    MessageQ_unregisterHeap(heap->id);
    free(heap);
}

uint32_t message_heap_id(MessageHeap *heap) {
    assert(heap); 
    return heap->id;
}
