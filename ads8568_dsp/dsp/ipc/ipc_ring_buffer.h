#ifndef _IPC_RING_BUFFER_H_
#define _IPC_RING_BUFFER_H_

#include "ring_buffer.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct _IPCRingBuffer IPCRingBuffer;

RingBuffer * ipc_ring_buffer_new(char *name);
void         ipc_ring_buffer_destroy(RingBuffer *buffer);

/*
 * 请求 size 大小的 buffer
 */
void       * ipc_ring_buffer_acquired(RingBuffer *buffer, unsigned int size);

/*
 * 释放 size 大小的 buffer
 */
void         ipc_ring_buffer_release(RingBuffer *buffer, unsigned int size);

#if defined __cplusplus
}
#endif
#endif
