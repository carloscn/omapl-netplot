#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <string>

/* package header files */
#include <ti/syslink/Std.h>     /* must be first */
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/SysLink.h>

#include <ti/ipc/MultiProc.h>

#include <ti/ipc/MessageQ.h>
#include <ti/ipc/SharedRegion.h>

#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/utils/Memory.h>

#include "syslink_ipc.h"
#include "message_heap.h"

class MessageQueue {
public:
    MessageQueue(uint32_t share_region_id,
                 const std::string &slave_queue_name,
                 const std::string &host_queue_name);

    ~MessageQueue();

    MessageQ_Msg msg_alloc(uint32_t size);
    void msg_free(void *msg);

    /*
     * 给 slave 发送消息
     */
    void push(void *msg);

    /*
     * 接收 slave 发送的消息，默认是阻塞的
     */
    MessageQ_Msg pop(uint32_t timeout_msce=MessageQ_FOREVER);

private:    
    SysLinkIPC *m_syslink;
    MessageHeap *m_heap;
    uint32_t m_heap_id;
    MessageQ_Handle m_queue_recv;
    MessageQ_QueueId m_queue_send;
};

#endif
