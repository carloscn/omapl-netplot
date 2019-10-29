#include "server.h"
#include "../shared/protocol.h"
#include "log.h"

Server::Server(unsigned short rate_K, unsigned channel, unsigned short port)
    : m_rate(rate_K * 1024),
    m_channel(channel),
    m_trigger(m_rate) {
    m_syslink = new SysLinkIPC("DSP"); 
    m_syslink->start();
    m_queue = new MessageQueue(SHARED_REGION_ID_MSG_QUEUE, MSG_QUEUE_NAME_SLAVE, MSG_QUEUE_NAME_HOST);
    m_buffer = new RingBuffer(m_syslink->get_proc_id(), RING_BUFFER_NAME, SHARED_REGION_ID_RING_BUFFER);
    m_controler = new Controler(m_queue);
    this->listen =new QTcpServer;
    this->listen->listen(QHostAddress::Any, port);
    LOG_INFO("listen on port %d", port);            

    QObject::connect(this->listen,SIGNAL(newConnection()),this,SLOT(processConnection()));
}

void Server::processConnection() {
    LOG_INFO("on connected");            
    this->client =this->listen->nextPendingConnection();
    QObject::connect(this->client,SIGNAL(readyRead()),this,SLOT(process_ready_read()));
}

void Server::process_ready_read() {
    int loop = 0;

    LOG_INFO("start");            
    m_controler->start(m_rate, m_trigger, m_channel);

    while (loop < 10) {
        /* 等待 slave 端通知，1s 超时 */
        Message *msg = (Message *)m_queue->pop(1000 /* 1 sec */); 

        if (! msg) {
            //LOG_TRACE("recv data timeout");
            continue;
        }

        if (msg->cmd != MSG_CMD_RAW_DATA && msg->cmd != MSG_CMD_FFT_DATA) {
            LOG_WARN("unexpected message");            
            m_queue->msg_free(msg);
            continue;
        }

        uint32_t size = sizeof(float) * m_trigger;
        char *data = (char *)m_buffer->acquired(size); /* 从 ipc ring buffer 读取数据 */
        if (data == NULL)
            LOG_FATAL("failed to acquired data");

        if (msg->cmd == MSG_CMD_RAW_DATA) {      /* 时域数据 */
            this->client->write(data, size);
            this->client->flush();
        }
        else if (msg->cmd == MSG_CMD_FFT_DATA) {  /* 频域数据 */
            LOG_DEBUG("unsupported");
        }

        m_buffer->release(size); 
        m_queue->msg_free(msg);

        LOG_DEBUG("send: %d", loop++);
    }

    LOG_INFO("send complete! exit ...");

    this->client->close();
    delete this->listen;

    m_controler->stop();
    delete m_buffer;
    delete m_queue;
    m_syslink->stop();
    delete m_syslink;

    exit(0);
}
