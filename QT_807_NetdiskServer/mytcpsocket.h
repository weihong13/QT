#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QWidget>
#include <QTcpSocket>
#include "protocol.h"
#include "reqhandler.h"


class MyTcpSocket:public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    // 获取 登录名
    QString& getLoginName();
    // 读出PDU
    PDU* readPDU();
    // 处理请求
    PDU* handleReq(PDU* pdu);

    // 发送PDU
    void sendPDU(PDU* pdu);



    // 请求处理器
    ReqHandler* m_rh;
    // 存储socket中的消息
    QByteArray m_buff;

public slots:
    // 接收消息的槽函数
    void recvMsg();
    // 处理下线的槽函数
    void clientOffline();

signals:
    // 下线的信号，将下线的客户端socket发出去。
    void offline(MyTcpSocket* mytcpsocket);

private:
    // 登陆成功的用户名
    QString m_LoginName;
};

#endif // MYTCPSOCKET_H
