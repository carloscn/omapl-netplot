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
#include "netsocket.h"
#include <QFile>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(uint32_t rate_K, uint32_t channel, QWidget *parent = 0);
    ~MainWindow();

    /* 重载 show */
    void show();

protected slots:
    void update_time_domain(std::vector<float> raw);
    void update_fre_domain(std::vector<float> fft);
    void tab_changed(int num);
    void on_net_change_sample_para(uint32_t,uint32_t);
    void on_net_start_sample();
    void on_net_stop_sample();

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
    netsocket *socket;
    bool is_start;
    QFile *file_handler;
    void reset_sample_para(uint32_t channel, uint32_t fs);


};

#endif
