#include <QTimer>
#include <QMessageBox>
#include <QLabel>
#include <cstdio>
#include <assert.h>
#include <iostream>
#include <vector>

#include "mainwindow.h"
#include "plot.h"
#include "../shared/protocol.h"
#include "log.h"
#include <stdio.h>

using namespace std;

MainWindow::MainWindow(uint32_t rate_K, uint32_t channel, QWidget *parent) :
    QMainWindow(parent),
    m_rate(rate_K * 1024),
    m_channel(channel),
    m_trigger(m_rate/15) {
    m_syslink = new SysLinkIPC("DSP"); 
    m_syslink->start();
    m_queue = new MessageQueue(SHARED_REGION_ID_MSG_QUEUE, MSG_QUEUE_NAME_SLAVE, MSG_QUEUE_NAME_HOST);
    m_buffer = new RingBuffer(m_syslink->get_proc_id(), RING_BUFFER_NAME, SHARED_REGION_ID_RING_BUFFER);
    m_receiver = new SampleReceiver(m_queue, m_buffer, m_trigger);
    m_controler = new Controler(m_queue);
    setWindowFlags(Qt::FramelessWindowHint);
    resize(QSize(800, 480));

    QTabWidget *tab = new QTabWidget(this); 
    tab->resize(QSize(800, 480));
    QObject::connect(tab, SIGNAL(currentChanged(int)), this, SLOT(tab_changed(int))); /* 注册 Tab  事件处理 */


    file_handler = new QFile();


    /* 时域 */
    Plot::Params params_td;
    params_td.x_min = m_rate / 2 - 256;
    params_td.x_max = m_rate / 2 + 256;
    params_td.x_step = 100;
    params_td.y_min = -10;
    params_td.y_max = 10;
    params_td.y_step = 1;
    m_time_domain = new Plot(params_td);
    tab->addTab(m_time_domain, "TimeDomain");

    /* 频域 */
    Plot::Params params_md;
    params_md.x_min = 0;
    params_md.x_max = m_rate;
    params_md.x_step = 2000;
    params_md.y_min = 0;
    params_md.y_max = 10;
    params_md.y_step = 1;
    m_fre_domain = new Plot(params_md);
    tab->addTab(m_fre_domain, "FreqDomain");

    /* exit */
    QLabel *label = new QLabel("Exit");  
    tab->addTab(label, "Exit");

    qRegisterMetaType<std::vector<float> >("std::vector<float>");

#if 1
    socket = new netsocket("NUll", 8877);

    QObject::connect( (QObject*)this->m_receiver, \
                     SIGNAL(raw(std::vector<float>)), \
                     (QObject*)this->socket, \
                     SLOT(on_dsp_data_recv(std::vector<float>)) );
    QObject::connect( (QObject*)this->socket, \
                     SIGNAL(change_sample_para(uint32_t,uint32_t)), \
                     this, \
                     SLOT(on_net_change_sample_para(uint32_t,uint32_t)) );
    QObject::connect( (QObject*)this->socket, \
                     SIGNAL(cmd_start_sample()), \
                     this, \
                     SLOT(on_net_start_sample()) );
    QObject::connect( (QObject*)this->socket, \
                     SIGNAL(cmd_stop_sample()), \
                     this, \
                     SLOT(on_net_stop_sample()) );
#endif

#if 0
    QObject::connect(m_receiver, SIGNAL(raw(std::vector<float>)), this, 
                    SLOT(update_time_domain(std::vector<float>)), 
                    Qt::BlockingQueuedConnection);
    QObject::connect(m_receiver, SIGNAL(fft(std::vector<float>)), this, 
                    SLOT(update_fre_domain(std::vector<float>)), 
                    Qt::BlockingQueuedConnection);
#endif
    m_controler->disable_fft();

}

void MainWindow::update_time_domain(vector<float> raw) {
    // update picture on ui
    //m_time_domain->update(&raw[0], raw.size());
    char* data_char = (char*)&raw[0];
    file_handler->setFileName("../adc.bin");
    file_handler->open( QIODevice::WriteOnly | QIODevice::Append );
    file_handler->write(data_char, raw.size() * 4);
    file_handler->close();
    qDebug() << "write data to document.";
}

void MainWindow::update_fre_domain(vector<float> fft) {
    // update pic on ui
    //m_fre_domain->update(&fft[0], fft.size());
}

void MainWindow::show() {
    QMainWindow::show(); 

    nice(-20);
    m_controler->start(m_rate, m_trigger, m_channel);
    //m_receiver->start(m_trigger);
    is_start = false;
}

void MainWindow::tab_changed(int num) {
    switch(num) {
    case 0: /* 时域 */
        LOG_INFO("stopsamp");
        //m_controler->disable_fft();
        //m_receiver->start(m_trigger);
        break;
    case 1: /* 频域 */
        LOG_INFO("startsamp");
       // m_controler->enable_fft();
        m_receiver->stop();
        break;
    case 2: /* exit */
        LOG_INFO("exit");

        m_controler->stop();
        delete m_receiver;
        delete m_buffer;
        delete m_queue;
        m_syslink->stop();
        delete m_syslink;
        delete m_time_domain;
        delete m_fre_domain;
        exit(0);
        break;
    default:
        ;
    }
}

void MainWindow::reset_sample_para(uint32_t channel, uint32_t fs)
{
    if (is_start == false ) {
        m_receiver->stop();
        m_controler->stop();
        m_rate = fs * 1024;
        m_channel = channel;
        m_trigger = 1024;
        m_controler->change_sample_rate(m_rate);
        m_receiver->start(m_trigger);
        is_start = true;
    }
}

void MainWindow::on_net_change_sample_para(uint32_t channel, uint32_t fs)
{
    if (channel < 5 ) {
        m_channel = channel;
    }
    if (fs < 500000) {
        m_rate = fs * 1024;
        m_trigger = m_rate;
    }
    reset_sample_para(channel,fs);
}
void MainWindow::on_net_start_sample()
{
    if (is_start == false) {
        m_receiver->start(m_trigger);
        is_start = true;
    }
}

void MainWindow::on_net_stop_sample()
{
    if (is_start == true) {
        m_receiver->stop();
        is_start = false;
    }
}

MainWindow::~MainWindow() { }
