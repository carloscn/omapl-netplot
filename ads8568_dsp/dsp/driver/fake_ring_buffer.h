#ifndef _FAKE_RING_BUFFER_H_
#define _FAKE_RING_BUFFER_H_

#include "ring_buffer.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct _FakeRingBuffer FakeRingBuffer;
struct _FakeRingBuffer {
    RingBuffer parent;
    float buffer[200 * 1024];
};

RingBuffer * fake_ring_buffer_new(char *name);
void         fake_ring_buffer_destroy(RingBuffer *buffer);

void       * fake_ring_buffer_acquired(RingBuffer *buffer, unsigned int size);
void         fake_ring_buffer_release(RingBuffer *buffer, unsigned int size);

#if defined __cplusplus
}
#endif
#endif
