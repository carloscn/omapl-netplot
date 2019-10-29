#ifndef _CONTROLER_H_
#define _CONTROLER_H_

#include <stdint.h>
#include "message_queue.h"

class Controler {
public:
    Controler(MessageQueue *queue);
    ~Controler();

    void start(uint32_t rate, uint32_t trigger, uint32_t channel);
    void stop();

    void enable_fft();
    void disable_fft();

    void change_sample_rate(uint32_t rate);

private:
    void send_cmd(uint32_t cmd);

private:
    MessageQueue *m_queue;
};

#endif
