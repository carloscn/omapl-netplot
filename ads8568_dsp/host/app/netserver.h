#ifndef NETSERVER_H
#define NETSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include "../shared/protocol.h"
class NetServer
{
public:
    NetServer(QString target_ip_str, QString target_port_str );


private :

    QTcpServer  *listener;
    QTcpSocket  *socketer;

public slots:

    void on_server_new_connected();
    void on_ready_read();
    void on_socket_disconnected();
};

#endif // NETSERVER_H
