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

    // 登陆成功的用户名
    QString m_LoginName;

public slots:
    // 接收消息的槽函数
    void recvMsg();
    // 处理下线的槽函数
    void clientOffline();

signals:
    // 下线的信号，将下线的客户端socket发出去。
    void offline(MyTcpSocket* mytcpsocket);

};

#endif // MYTCPSOCKET_H
