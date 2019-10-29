#ifndef _MESSAGE_HEAP_H_
#define _MESSAGE_HEAP_H_

/* xdctools header files */

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct _MessageHeap MessageHeap;

MessageHeap * message_heap_new(uint32_t shared_region_id);
void          message_heap_destroy(MessageHeap *heap);
uint32_t      message_heap_id(MessageHeap *heap);

#if defined __cplusplus
}
#endif
#endif
