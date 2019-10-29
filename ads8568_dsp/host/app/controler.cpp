#include "controler.h"
#include "log.h"
#include "../shared/protocol.h"

Controler::Controler(MessageQueue *queue) : m_queue(queue) { }

Controler::~Controler() { }

void Controler::start(uint32_t rate, uint32_t trigger, uint32_t channel) {
    Message *msg = (Message *)m_queue->msg_alloc(MSG_SIZE);
    msg->cmd = MSG_CMD_START;
    msg->rate = rate;
    msg->trigger = trigger;
    msg->channel = channel;

    LOG_DEBUG("set sampling rate as %d", msg->rate);
    LOG_DEBUG("set sampling channel as %d", msg->channel);

    m_queue->push(msg);
}

void Controler::stop() {
    LOG_INFO("stop ad");
    send_cmd(MSG_CMD_STOP);
}

void Controler::enable_fft() {
    LOG_INFO("enable fft");
    send_cmd(MSG_CMD_ENABLE_FFT);
}

void Controler::disable_fft() {
    LOG_INFO("disable fft");
    send_cmd(MSG_CMD_DISABLE_FFT);
}

void Controler::send_cmd(uint32_t cmd) {
    Message *msg = (Message *)m_queue->msg_alloc(MSG_SIZE);
    msg->cmd = cmd;
    m_queue->push(msg);
}

void Controler::change_sample_rate(uint32_t rate) {
    LOG_INFO("set sample rate as %d", rate);
    Message *msg = (Message *)m_queue->msg_alloc(MSG_SIZE);
    msg->cmd = MSG_CMD_SAMPLE_RATE;
    msg->rate = rate;
    m_queue->push(msg);
}
