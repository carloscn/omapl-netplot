#include "netclientthread.h"
#include "log.h"

NetClientThread::NetClientThread( QString target_id, int port )
{

    this->data_ready = false;
    this->socket = new QTcpSocket( this);
    this->server = new QTcpServer( this );
    // socket server
    int ret = this->server->listen(QHostAddress::Any,port);
    connect( this->server, SIGNAL(newConnection()), this,SLOT(on_new_connect()) );

    if( !ret ) {
        qDebug() << this->server->errorString();
    }else {
        qDebug() << "listener has been set up!\n";
    }
}

void    NetClientThread::run()
{
    while( false ) {
        //LOG_DEBUG("join the thread.!!!\n");
        if( this->data_ready == true ) {
            LOG_DEBUG("Send buffer.....!!!\n");
            this->socket->write(send_buffer);
            send_buffer.clear();
            this->data_ready = false;
        }
    }
}

void    NetClientThread::on_get_ready_datas( float *data, int length )
{
    QByteArray block;
    QDataStream iostream(&block,QIODevice::ReadWrite);
    iostream.setVersion(QDataStream::Qt_4_0);
    iostream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    for(int i = 0; i < length; i ++) {
        iostream << (float) (*(data + i));
    }

    this->socket->write(block,block.size());
}

void    NetClientThread::on_new_connect()
{
    this->socket = this->server->nextPendingConnection();
    connect( this->socket, SIGNAL(readyRead()),this,SLOT(on_read_message()) );
    LOG_DEBUG("A new client join.");
}

void    NetClientThread::on_read_message()
{
    QByteArray recv;
    char *recv_ch;
    recv.clear();
    recv = this->socket->readAll();
    recv_ch = recv.data();
    if( !(recv_ch[0] == 0xAA && recv_ch[1] == 0xBB) ) {
        return ;
    }
    char cmd = recv_ch[3];
    if( cmd == 0x01 ) {
        emit close_activity();
    }
}
