#ifndef NETSOCKET_H
#define NETSOCKET_H

#include <stdint.h>
#include <vector>
#include <QThread>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QByteArray>
#include <QHostInfo>
#include <QNetworkInterface>


using namespace std;

class netsocket : public QThread
{
    Q_OBJECT
public:
    netsocket( QString target_ip, quint16 port );
    netsocket();
    ~netsocket();
    void stop();

private:
    QTcpSocket  *socket;
    QTcpServer  *server;
    bool* is_enable_sample;
    quint64 packet_number;

protected:
    void    run();

public slots:
    void on_new_connect();
    void on_dsp_data_recv(std::vector<float>);
    void on_get_ready_datas( float *data, int length );

private slots:
    void on_read_message();

signals:
    void change_sample_para(uint32_t,uint32_t);
    void cmd_start_sample();
    void cmd_stop_sample();
};
#endif // NETSOCKET_H
