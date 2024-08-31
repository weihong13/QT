#include "clienttask.h"

#include <QThread>

ClientTask::ClientTask()
{

}

ClientTask::ClientTask(MyTcpSocket *socket):m_socket(socket)
{

}

void ClientTask::run()
{
    // 将socket中，接收到信息触发的信号，与取出信息的信号槽函数进行关联
    connect(m_socket,&MyTcpSocket::readyRead,m_socket,&MyTcpSocket::recvMsg);

    // 将socket中，接收客户端关闭的信号，与处理下线的槽函数进行关联
    connect(m_socket,&MyTcpSocket::disconnected,m_socket,&MyTcpSocket::clientOffline);
    // 将socket放入线程中，在线程中处理socket的内容
    m_socket->moveToThread(QThread::currentThread());

}
