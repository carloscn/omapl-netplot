#include "message_queue.h"
#include <string.h>
#include "log.h"

MessageQueue::MessageQueue(uint32_t share_region_id,
                           const std::string &slave_queue_name,
                           const std::string &host_queue_name) {
    m_heap = message_heap_new(share_region_id);
    m_heap_id = message_heap_id(m_heap);

    MessageQ_Params params;
    MessageQ_Params_init(&params);
    m_queue_recv = MessageQ_create(strdup(host_queue_name.c_str())/*TODO: free it */, &params);

    /* 将由 slave 端建立 */
    m_queue_send = MessageQ_INVALIDMESSAGEQ; 

    int status;
    do {
        status = MessageQ_open(strdup(slave_queue_name.c_str()), &m_queue_send);
        if (status == MessageQ_E_NOTFOUND) 
            sleep(1);
    } while (status == MessageQ_E_NOTFOUND);

    if (status < 0) 
        LOG_FATAL("failed to open message queue %s", slave_queue_name.c_str());
}

MessageQueue::~MessageQueue() {
    MessageQ_delete(&m_queue_recv);
    MessageQ_close(&m_queue_send);
    message_heap_destroy(m_heap);
}

MessageQ_Msg MessageQueue::msg_alloc(uint32_t size) {
    MessageQ_Msg msg = MessageQ_alloc(m_heap_id, size);
    if (msg == NULL)
        LOG_FATAL("failed to malloc message");

    return msg;
}

void MessageQueue::msg_free(void *msg) {
    ASSERT(msg);
    MessageQ_free((MessageQ_Msg)msg);
}

void MessageQueue::push(void *msg) {
    MessageQ_setReplyQueue(m_queue_recv, (MessageQ_Msg)msg);
    MessageQ_put(m_queue_send, (MessageQ_Msg)msg);
}

MessageQ_Msg MessageQueue::pop(uint32_t timeout_msec) {
    MessageQ_Msg msg = NULL;
    MessageQ_get(m_queue_recv, &msg, timeout_msec);
    return msg;
}
