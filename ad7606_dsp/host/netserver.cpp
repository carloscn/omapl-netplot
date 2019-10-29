#include "netserver.h"

netserver::netserver()
{
    mutex = new QMutex();
    server = new QTcpServer();
    socket = new QTcpSocket();
}

