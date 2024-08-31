#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QWidget>
#include <QTcpServer>
#include <mytcpsocket.h>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    // 获取单例对象
    static MyTcpServer& getInstance();
    // 重写连接成功的槽函数
    void incomingConnection(qintptr handle);

    // 转发函数
    void resend(char* tarName,PDU* pdu);

public slots:
    // 删除已下线客户端socket的槽函数
    void deleteSocket(MyTcpSocket* mytcpsocket);

private:
    // 构造私有化
    MyTcpServer();
    // 删除拷贝构造
    MyTcpServer(const MyTcpServer& instance) = delete;
    // 删除 拷贝赋值运算符
    MyTcpServer operator =(const MyTcpServer&) =delete;
    // 定义连接成功的客户端 socket 的列表
    QList<MyTcpSocket*> m_tcpSocketList;
    // 线程池对象
    QThreadPool m_threadPool;
};

#endif // MYTCPSERVER_H
