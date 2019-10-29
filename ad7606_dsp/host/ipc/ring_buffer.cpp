#include <stdlib.h>
#include <string.h>
#include "ring_buffer.h"
#include <ti/syslink/RingIOShm.h>
#include "log.h"

RingBuffer::RingBuffer(uint32_t proc, const std::string &name, uint32_t share_region)
    : m_alive(false) {
    RingIOShm_Params  params_create;
    RingIOShm_Params_init(&params_create);
    params_create.commonParams.name     = strdup(name.c_str());
    params_create.ctrlRegionId          = share_region;
    params_create.dataRegionId          = share_region;
    params_create.attrRegionId          = share_region;
    params_create.attrSharedAddrSize    = 16;
    params_create.dataSharedAddrSize    = 0x900000;      /* 9M 大小的数据区，最大为10M，这个可以在配置文件中配置 (../shared/config.bld) */
    params_create.remoteProcId          = proc;
    params_create.gateHandle            = NULL;          /* 使用默认的 Gate */

    m_handle = RingIO_create(&params_create);
    if(m_handle == NULL) {
        LOG_ERROR("failed to create ring io %s", name.c_str());
        m_alive = false;
    }
    else {
        RingIO_openParams  params_open;
        params_open.openMode = RingIO_MODE_READER;
        params_open.flags = 0;
        if (RingIO_open (strdup(name.c_str()), &params_open, NULL, &m_read_handle) < 0) { /* 读模式打开 */
            LOG_ERROR("failed to open ring io");
            m_alive = false;
        }
        LOG_DEBUG("%s is ready", name.c_str());
        m_alive = true;
    }
}

RingBuffer::~RingBuffer() {
    if (m_alive) {
        RingIO_close(&m_read_handle);
        RingIO_delete(&m_handle);
    }
}

void *RingBuffer::acquired(uint32_t size) {
    if (! m_alive) {
        LOG_WARN("ring buffer is not alive");
        return NULL;
    }

    uint8_t *buf;
    if (RingIO_acquire(m_read_handle, (RingIO_BufPtr*)&buf, &size) != RingIO_S_SUCCESS) {
        LOG_ERROR("unstatified acquired, max is %d", size);
        return NULL;
    }

    return (void *)buf;
}

void RingBuffer::release(uint32_t size) {
    if (! m_alive) {
        LOG_WARN("ring buffer is not alive");
        return;
    }

    if (RingIO_release(m_read_handle, size) < 0)
        LOG_ERROR("failed to release buffer");
}
