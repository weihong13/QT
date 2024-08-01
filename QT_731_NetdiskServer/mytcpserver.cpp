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

    // 将 下线信号 和 删除已下线客户端socket的槽函数 进行关联
    connect(tcpSocket,&MyTcpSocket::offline,this,&MyTcpServer::deleteSocket);
}

// 转发函数，将pdu转发给，目标用户
void MyTcpServer::resend(char *tarName, PDU *pdu)
{
    qDebug()<<"resend tarName: "<<tarName;
    // 在 m_tcpSocketList 列表中，查找要转发的目标用户
    foreach(MyTcpSocket* tcpSocket, m_tcpSocketList)
    {
        if(tcpSocket->m_LoginName == tarName)
        {
            tcpSocket->write((char*)pdu,pdu->uiPDULen);
            return;
        }
    }

}

// 删除已下线客户端socket的槽函数--将已下线的socket，从socket列表（m_tcpSocketList）中删除
void MyTcpServer::deleteSocket(MyTcpSocket *mytcpsocket)
{
    // 将已下线的socket，从socket列表（m_tcpSocketList）中删除
    m_tcpSocketList.removeOne(mytcpsocket);
    // 在客户端与服务器建立连接时，我们是在堆区new 出了一块空间来 存储新的socket，因此要释放掉
    // 这里是不能使用delete释放的，因为即使客户端关闭了，是不会立马关闭的，得等一会才能关闭
    // 因此得调用等待关闭的函数
    mytcpsocket->deleteLater();
    mytcpsocket = NULL;

    // 测试 是否移除成功
    qDebug()<<"m_tcpSocketList size: "<<m_tcpSocketList.size();

    for(int i= 0;i<m_tcpSocketList.size();i++)
    {
        qDebug()<<"m_LoginName: "<<m_tcpSocketList[i]->m_LoginName;
    }

}
