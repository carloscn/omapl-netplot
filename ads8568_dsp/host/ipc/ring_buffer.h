#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_


#include <stdint.h>
#include <string>

/* package header files */
#include <ti/syslink/Std.h>     /* must be first */
#include <ti/syslink/RingIO.h>

class RingBuffer {
public:
    RingBuffer(uint32_t proc, const std::string &name, uint32_t share_region);
    ~RingBuffer();

    /*
     * 请求 size 大小的 buffer 
     */
    void *acquired(uint32_t size);

    /*
     * 释放 size 大小的 buffer
     */
    void release(uint32_t size);

private:
    bool m_alive;
    RingIO_Handle m_handle;
    RingIO_Handle m_read_handle;
};

#endif
