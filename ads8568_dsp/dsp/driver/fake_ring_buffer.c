#include <stdlib.h>
#include <string.h>

#include <ti/syslink/Std.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/SysLink.h>

#include "log.h"
#include "fake_ring_buffer.h"

RingBuffer *fake_ring_buffer_new(char *name) {
    FakeRingBuffer *buffer = (FakeRingBuffer *)malloc(sizeof(FakeRingBuffer));
    memset(buffer, 0, sizeof(FakeRingBuffer));

    /* 绑定子类实现 */
    buffer->parent.destroy = fake_ring_buffer_destroy;
    buffer->parent.acquired = fake_ring_buffer_acquired;
    buffer->parent.release = fake_ring_buffer_release;

    return &buffer->parent;
}

void fake_ring_buffer_destroy(RingBuffer *base) {
    if (! base)
        return;

    free(base);
}

void *fake_ring_buffer_acquired(RingBuffer *base, unsigned int size) {
    ASSERT(base);
    FakeRingBuffer *buffer = (FakeRingBuffer *)base;
    return (void *)buffer->buffer;
}

void fake_ring_buffer_release(RingBuffer *base, unsigned int size) {
    ASSERT(base);
}
