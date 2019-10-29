#ifndef NETCLIENTTHREAD_H
#define NETCLIENTTHREAD_H

#include <QThread>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QByteArray>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QMessageBox>
class NetClientThread : public QThread
{
    Q_OBJECT
public:
    NetClientThread( QString target_ip, int port );
    void stop();

private:
    QTcpSocket  *socket;
    QTcpServer  *server;
    QByteArray  send_buffer;
    bool        data_ready;
    float       *data_buffer;
    int         data_length;
signals:
    void close_activity();

public slots:
    void on_get_ready_datas( float *data, int length );
    void on_new_connect();
    void on_read_message();
protected:
    void    run();
private slots:


};

#endif // NETCLIENTTHREAD_H
