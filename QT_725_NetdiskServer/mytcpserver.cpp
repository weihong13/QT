#include "mytcpserver.h"

#include <QMessageBox>

// 构造函数
MyTcpServer::MyTcpServer()
{

}

// 获取单例对象
MyTcpServer &MyTcpServer::getInstance()
{
    // 设置静态局部引用
    static MyTcpServer instance;
    // 返回单例对象
    return instance;
}

// 重写连接成功的信号槽函数，并将连接成功的socket 放入 m_tcpSocketList 列表中。
void MyTcpServer::incomingConnection(qintptr handle)
{

    // 展示连接成功。
    qDebug()<<"新客户端连接成功";
    // 将新连接的客户端socket 存到 m_tcpSocketList 列表中。
    MyTcpSocket* tcpSocket = new MyTcpSocket;
    tcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(tcpSocket);
}
