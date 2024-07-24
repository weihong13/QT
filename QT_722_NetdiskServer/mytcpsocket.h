#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QWidget>
#include <QTcpSocket>
#include "protocol.h"


class MyTcpSocket:public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
public slots:
    void recvMsg();
};

#endif // MYTCPSOCKET_H
