#ifndef _SAMPLE_RECEIVER_H_
#define _SAMPLE_RECEIVER_H_


#include "thread.h"
#include "message_queue.h"
#include <stdint.h>
#include <QThread>
#include <vector>

class RingBuffer;

class SampleReceiver : public QObject, public base::ThreadWorker {
    Q_OBJECT

public:
    SampleReceiver(MessageQueue *queue, RingBuffer *buffer, int trigger);
    ~SampleReceiver();

    /*
     * @trigger 采样的点数
     */
    void start(uint32_t trigger); 
    void stop();

signals:
    void raw(std::vector<float> sample);
    void fft(std::vector<float> sample);

private:
    void *run();

private:
    base::Thread *m_thread;
    MessageQueue *m_msg_queue;
    volatile bool m_running;
    RingBuffer *m_buffer;
    uint32_t m_trigger;
};

#endif
