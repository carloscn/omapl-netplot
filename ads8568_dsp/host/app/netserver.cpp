#include "netserver.h"


// This consider as client
NetServer::NetServer(QString target_ip_str, QString target_port_str)
{

    this->listener  =   new QTcpServer();
    //this->socketer  =   new QTcpSocket(this);
    connect( this->listener,SIGNAL(newConnection()),this,SLOT( on_server_new_connected()) );
    int ret =   this->listener->listen( QHostAddress::Any,target_port_str.toInt() );
    if( !ret )
        int i;


}

NetServer::on_server_new_connected()
{
    auto socket_temp    =   this->listener->nextPendingConnection();
    this->socketer  =   socket_temp;
    connect( socket_temp, SIGNAL(readyRead()),this, SLOT(on_ready_read()) );
    connect( socket_temp, SIGNAL(disconnected()), this, SLOT(on_socket_disconnected()) );
}

NetServer::on_socket_disconnected()
{

}
