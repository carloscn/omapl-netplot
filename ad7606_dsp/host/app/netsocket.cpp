#include "netsocket.h"
#include "log.h"
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <queue>
#include <vector>
#include <sys/time.h>
#include <time.h>

using namespace std;

netsocket::netsocket()
{
}


netsocket::netsocket( QString target_id, quint16 port )
{
    this->server = new QTcpServer( this );
    this->socket = new QTcpSocket();
    // socket server
    int ret = this->server->listen(QHostAddress::Any, port);
    packet_number = 0;
    if( !ret ) {
        qDebug() << this->server->errorString();
    }else {
        QObject::connect( this->server, SIGNAL(newConnection()), this, SLOT(on_new_connect()) );
        qDebug() << "listener has been set up! ";
        qDebug() << "ip: " << "any.";
        qDebug() << "port: " << port;
    }
}
netsocket::~netsocket()
{

}
void    netsocket::run()
{
}

void    netsocket::on_get_ready_datas( float *data, int length )
{
}

void    netsocket::on_new_connect()
{
    this->socket = this->server->nextPendingConnection();
    LOG_INFO("A new client join.");
    QObject::connect( (QObject*)this->socket, SIGNAL(readyRead()),this,SLOT(on_read_message()) );
    //socket->write("@OMAPL138: Server has been set up!\n");
}
// 0xAA 0xBB 0xlen.3 0xcmd 0xdata1 0xdata2 0xdata3 0xCC 0xDD
#define         NET_CMD_START_SAMPLE        0x04
#define         NET_CMD_STOP_SAMPLE         0x08
#define         NET_CMD_SWITCH_CHANNEL      0x03
#define         NET_CMD_SET_FS              0xA1
#define         NET_CMD_HEADER_1            0xAA
#define         NET_CMD_HEADER_2            0xBB

void    netsocket::on_read_message()
{
    QByteArray socket_rom = socket->readAll();
    uint16_t cmd_length = 0;
    uint32_t cmd_type = 0;
    uint32_t fs;
    uint32_t channel;
    LOG_DEBUG("net received data len: %d", socket_rom.length());
    if ((socket_rom.at(0) == 0xAA) && (socket_rom.at(1) == 0xBB)) {
        cmd_length = (socket_rom.at(2) << 8) | (socket_rom.at(3)) ;
        cmd_type = socket_rom.at(4);
        uint8_t *cmd_data = new uint8_t[cmd_length];
        memset(cmd_data,0,cmd_length);
        for (uint16_t i = 0 ; i < cmd_length; i ++) {
            cmd_data[i] |= socket_rom.at(5 + i);
        }

        switch(cmd_type){
        case NET_CMD_START_SAMPLE:
            LOG_DEBUG("start sample!\n");
            emit cmd_start_sample();
            break;

        case NET_CMD_STOP_SAMPLE:
            LOG_DEBUG("stop sample!\n");
            emit cmd_stop_sample();
            break;

        case NET_CMD_SET_FS:
            channel = 10;
            fs = (cmd_data[0] << 8) | (cmd_data[1] << 0);
            emit change_sample_para(channel,fs*1000);
            LOG_DEBUG("net cmd set fs : %d KHz\n", fs);
            break;

        case NET_CMD_SWITCH_CHANNEL:

            fs = 999999999;
            channel = (cmd_data[0] << 8) | (cmd_data[1] << 16);
            emit change_sample_para(channel,fs);
            LOG_DEBUG("net cmd switch channel : %d\n", channel);
            break;
        }

    } else {
        LOG_DEBUG("Socket data not align!\n");
        return;
    }

}
void    netsocket::on_dsp_data_recv(std::vector<float> raw)
{
    QByteArray send_rom;
    send_rom.clear();
    union result {
        quint64 u_64;
        float f_64;
        char c_8[8];
    } r;

    char header[2];
    char tail[2];
    header[0] = 'a';
    header[1] = 'b';
    tail[0] = 'c';
    tail[1] = 'd';
    size_t size = raw.size();
    send_rom.append(header,2);
    r.u_64 = packet_number;
    send_rom.append(r.c_8,8);
    r.u_64 = size;
    send_rom.append(r.c_8,8);
    for (quint64 i = 0; i < size; i ++) {
        r.f_64 = (double)raw.at(i);
        send_rom.append(r.c_8,8);
    }
    send_rom.append(tail,2);
    LOG_DEBUG("send packet length = %g", size * 8.0 + 4 + 16);
    socket->write(send_rom);
    socket->flush();
    packet_number ++;
}

