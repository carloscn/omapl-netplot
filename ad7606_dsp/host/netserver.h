#ifndef NETSERVER_H
#define NETSERVER_H

#include <QThread>
#include <QMutex>
#include <QString>
#include <QDebug>
#include <QtNetwork>
#include <QtNetwork/QNetworkInterface>

class netserver : public QThread
{
public:
    netserver();

    void stop();

protected:

    void run();

private:

    QMutex *mutex;
    QTcpServer *server;
    QTcpSocket *socket;

};

#endif // NETSERVER_H
