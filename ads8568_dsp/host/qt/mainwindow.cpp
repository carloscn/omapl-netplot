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
    m_trigger(m_rate) {

    QString server_ip = "192.168.1.135";
    quint16     server_port =   8322;

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

    this->socket_thread = new NetClientThread( server_ip, server_port );
    connect(this,SIGNAL(write_ready(float*,int)),this->socket_thread,SLOT(on_get_ready_datas(float*,int)) );
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

    QObject::connect(m_receiver, SIGNAL(raw(std::vector<float>, size_t)), this,
                     SLOT(update_time_domain(std::vector<float>,  size_t)),
                     Qt::BlockingQueuedConnection);
    QObject::connect(m_receiver, SIGNAL(fft(std::vector<float>, size_t)), this,
                     SLOT(update_fre_domain(std::vector<float>, size_t)),
                     Qt::BlockingQueuedConnection);
    QObject::connect( socket_thread,
                      SIGNAL(close_activity()),
                      this,
                      SLOT(on_close_app()) );
    LOG_DEBUG("net server thread start....");
    //socket_thread->start();
}

void MainWindow::update_time_domain(vector<float> raw, size_t length) {
    m_time_domain->update(&raw[0], raw.size());
    emit write_ready(&raw[0],length);

    //this->socket_write(&raw[0], raw.size());
}

void MainWindow::update_fre_domain(vector<float> fft, size_t length ) {
    m_fre_domain->update(&fft[0], fft.size());
    emit write_ready(&fft[0],length);
    //this->socket_write(&fft[0], fft.size());
}

void MainWindow::show() {
    QMainWindow::show();

    nice(-20);
    m_controler->start(m_rate, m_trigger, m_channel);
    m_receiver->start(m_trigger);
}

void MainWindow::tab_changed(int num) {
    switch(num) {
    case 0: /* 时域 */
        LOG_INFO("time domain");
        m_controler->disable_fft();
        break;
    case 1: /* 频域 */
        LOG_INFO("freq domain");
        m_controler->enable_fft();
        break;
    case 2: /* exit */
        this->on_close_app();
        break;
    default:
        ;
    }
}
void MainWindow::socket_write(float *data, quint16 length)
{

    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);

    for(int i = 0; i < length; i ++) {
        out << *(data+i);
        if( i != length - 1 )
            out << ',';
    }
    out.device()->seek(0);
    this->socket->write(block);

}
MainWindow::~MainWindow() {
    this->socket_thread->quit();
}

void MainWindow::on_close_app()
{
    LOG_INFO("exit");
    m_receiver->stop();
    m_controler->stop();
    delete m_receiver;
    delete m_buffer;
    delete m_queue;
    m_syslink->stop();
    delete m_syslink;
    delete m_time_domain;
    delete m_fre_domain;
    socket->flush();
    socket->close();
    this->socket_thread->quit();
    exit(0);
}
