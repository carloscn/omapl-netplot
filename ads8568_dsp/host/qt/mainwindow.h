#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include "plot.h"
#include "sample_receiver.h"
#include "controler.h"
#include "message_queue.h"
#include "ring_buffer.h"
#include "syslink_ipc.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include "netclientthread.h"
#include <QDataStream>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(uint32_t rate_K, uint32_t channel, QWidget *parent = 0);
    ~MainWindow();

    /* 重载 show */
    void show();
    void socket_write( float *data, quint16 length );

signals:
    void write_ready(float *data, int length);

protected slots:
    void update_time_domain(std::vector<float> raw, size_t lenght);
    void update_fre_domain(std::vector<float> fft, size_t lenght);
    void tab_changed(int num);
    void on_close_app();

private:
    Plot *m_time_domain;
    Plot *m_fre_domain;
    uint32_t m_rate;
    uint32_t m_channel;
    uint32_t m_trigger;

    SysLinkIPC *m_syslink;
    MessageQueue *m_queue;
    RingBuffer *m_buffer;
    SampleReceiver *m_receiver;
    Controler *m_controler;
    NetClientThread *socket_thread;
    QTcpSocket  *socket;
};

#endif
