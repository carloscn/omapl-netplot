#include <stdlib.h>
#include <string.h>

#include <ti/syslink/Std.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/SysLink.h>

#include "log.h"
#include "ipc_ring_buffer.h"

struct _IPCRingBuffer {
    RingBuffer parent;
    RingIO_Handle handle;  
    RingIO_Handle write_handle;  
};

RingBuffer *ipc_ring_buffer_new(char *name) {
    IPCRingBuffer *buffer = (IPCRingBuffer *)malloc(sizeof(IPCRingBuffer));
    memset(buffer, 0, sizeof(IPCRingBuffer));

    /* 绑定子类实现 */
    buffer->parent.destroy = ipc_ring_buffer_destroy;
    buffer->parent.acquired = ipc_ring_buffer_acquired;
    buffer->parent.release = ipc_ring_buffer_release;

    SysLink_setup(); 
    RingIO_openParams params_open;
    params_open.openMode = RingIO_MODE_WRITER;
    params_open.flags  = 2; /* 允许分配地址不连续的 buffer */

    /* 以写模式打开 */
    if (RingIO_open (name, &params_open, NULL, &buffer->write_handle) < 0)
        LOG_FATAL("fail to open ring buffer %s", name); 

    return &buffer->parent;
}

void ipc_ring_buffer_destroy(RingBuffer *base) {
    if (! base)
        return;

    IPCRingBuffer *buffer = (IPCRingBuffer *)base;

    RingIO_close(&buffer->write_handle);
    SysLink_destroy();
    free(buffer);
}

void *ipc_ring_buffer_acquired(RingBuffer *base, unsigned int size) {
    ASSERT(base);

    IPCRingBuffer *buffer = (IPCRingBuffer *)base;

    char *buf;
    unsigned int actual = size;
    if (RingIO_acquire(buffer->write_handle, (RingIO_BufPtr* )&buf, &actual) != RingIO_S_SUCCESS) {
        LOG_DEBUG("unstatified acquired, request is %d, actual is %d", size, actual);
        LOG_DEBUG("empty size is %d", RingIO_getEmptySize(buffer->write_handle));
        LOG_DEBUG("valid size is %d", RingIO_getValidSize(buffer->write_handle));
        return NULL;
    }

    return (void *)buf;
}

void ipc_ring_buffer_release(RingBuffer *base, unsigned int size) {
    ASSERT(base);

    IPCRingBuffer *buffer = (IPCRingBuffer *)base;

    if (RingIO_release(buffer->write_handle, size) < 0)
        LOG_ERROR("failed to release buffer, size is %d", size);
}
