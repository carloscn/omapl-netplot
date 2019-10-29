#include "ring_buffer.h"
#include "log.h"

void ring_buffer_destroy(RingBuffer *buffer) {
    ASSERT(buffer);
    buffer->destroy(buffer);  /* 调用子类具体实现 */
}

void *ring_buffer_acquired(RingBuffer *buffer, unsigned int size) {
    ASSERT(buffer);
    return buffer->acquired(buffer, size); /* 调用子类具体实现 */
}

void ring_buffer_release(RingBuffer *buffer, unsigned int size) {
    ASSERT(buffer);
    buffer->release(buffer, size);  /* 调用子类具体实现 */
}
