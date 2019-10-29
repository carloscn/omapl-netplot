#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <queue>
#include <vector>
#include <sys/time.h>
#include <time.h>

#include "sample_receiver.h"
#include "../shared/protocol.h"
#include "log.h"
#include "ring_buffer.h"

using namespace std;
using namespace base;

SampleReceiver::SampleReceiver(MessageQueue *queue, RingBuffer *buffer, int trigger) 
    : m_thread(new Thread(this, "sample_receiver")), 
      m_msg_queue(queue), 
      m_running(false),
      m_buffer(buffer),
      m_trigger(trigger) { }

SampleReceiver::~SampleReceiver() {
    m_running = false;
    m_thread->join(); /* 等待线程退出 */

    delete m_thread;
}

void SampleReceiver::start(uint32_t trigger) {
    assert(! m_running);
    m_trigger = trigger;
    m_thread->set_priority(0);
    m_thread->start();
}

void SampleReceiver::stop() {
    assert(m_running);
    m_running = false;
}

void *SampleReceiver::run() {
    LOG_INFO("sample receiver is running");
    m_running = true;

    while (m_running) {
        /* 等待 slave 端通知，1s 超时 */
        Message *msg = (Message *)m_msg_queue->pop(1000 /* 1 sec */);
        if (! m_running) {  /* 是否现在需要 quit */
            if (msg)
                m_msg_queue->msg_free(msg);
            break;
        }

        if (! msg) {
            //LOG_TRACE("recv data timeout");
            continue;
        }

        if (msg->cmd != MSG_CMD_RAW_DATA && msg->cmd != MSG_CMD_FFT_DATA) {
            LOG_WARN("unexpected message");            
            m_msg_queue->msg_free(msg);
            continue;
        }

        uint32_t size = sizeof(float) * m_trigger;
        float *data = (float *)m_buffer->acquired(size); /* 从 ipc ring buffer 读取数据 */
        if (data == NULL)
            LOG_FATAL("failed to acquired data");

        vector<float> samples(m_trigger, 0); 
        memcpy(&samples[0], data, sizeof(float) * samples.size());
        if (msg->cmd == MSG_CMD_RAW_DATA)       /* 时域数据 */
            emit raw(samples, samples.size());

        else if (msg->cmd == MSG_CMD_FFT_DATA)  /* 频域数据 */
            emit fft(samples, samples.size());

        m_buffer->release(size); 
        m_msg_queue->msg_free(msg);
    }

    m_running = false;
    LOG_INFO("sample receiver is stoped");

    return (void *)0;
}
