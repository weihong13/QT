#include "mytcpsocket.h"
#include "operatedb.h"

MyTcpSocket::MyTcpSocket()
{
    // 新建一个消息处理器成员
    m_rh = new ReqHandler();

    // 将socket中，接收到信息触发的信号，与取出信息的信号槽函数进行关联
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);

    // 将socket中，接收客户端关闭的信号，与处理下线的槽函数进行关联
    connect(this,&MyTcpSocket::disconnected,this,&MyTcpSocket::clientOffline);
}

MyTcpSocket::~MyTcpSocket()
{
    if(!m_rh) delete m_rh;
}

QString &MyTcpSocket::getLoginName()
{
    return m_LoginName;
}

PDU *MyTcpSocket::readPDU()
{
    // 打印socket中的数据长度
    qDebug()<<"socket中接收到的数据长度："<<this->bytesAvailable();

    // 读出消息结构体的总长度，这部分内容会从socket中读出去
    uint uiPDULen = 0; // 传出参数
    // 读取的大小为 消息结构体总长度的数据类型大小
    this->read((char*)&uiPDULen,sizeof(uint));

    // 计算实际消息长度--柔性数组长度(消息结构体总长度-结构体大小)
    uint uiMsgLen = uiPDULen-sizeof(PDU);
    // 初始化一个PUD结构体，用于存储
    PDU* pdu = initPDU(uiMsgLen);

    // 读出socket中剩余的数据，消息结构体的总长度已经被读出去了，剩下的不是一个完整的PDU结构体。
    // 因此需要对pdu这个指针进行偏移，偏移的长度就是已经读出的数据长度（消息结构体总长度的数据类型大小）
    // 而剩余需要读出的内容为，消息结构体总长度 减去 消息结构体总长度的数据类型大小
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

    // 测试--打印输出消息结构体的内容
//    qDebug()<<"/n/n/nreadPDU 结构体总长度："<<pdu->uiPDULen;
//    qDebug()<<"readPDU 消息类型："<<pdu->uiMsgType;
//    qDebug()<<"readPDU 消息长度："<<pdu->uiMsgLen;
//    qDebug()<<"readPDU 参数1："<<pdu->caData;
//    qDebug()<<"readPDU 参数2："<<pdu->caData+32;
//    qDebug()<<"readPDU 接收到的消息："<<pdu->caMsg;

    return pdu;
}

PDU *MyTcpSocket::handleReq(PDU *pdu)
{
    m_rh->m_pdu = pdu;
    // 根据消息类型对消息进行处理
    switch (pdu->uiMsgType)
    {
        // 注册请求
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            return m_rh->regist();
        }
        // 登录请求
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            return  m_rh->login(m_LoginName);
        }

        // 查找用户请求
        case ENUM_MSG_TYPE_FIND_USER_REQUEST:
        {
            return m_rh->findUser();
        }

        // 在线用户请求
        case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:
        {
            return m_rh->onlineUser();
        }
        // 添加用户请求
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            return m_rh->addFriend();
        }
        // 同意添加用户请求
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST:
        {
            m_rh->addFriendAgree();
        }

        // 刷新好友请求
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            return m_rh->flushFriend(m_LoginName);
        }


        default:
            break;
    }
    return NULL;
}

void MyTcpSocket::sendPDU(PDU *resPdu)
{
    qDebug()<<"recvMsg sendPDU ret";
    // 利用socket 向客户端发送 注册的响应
    write((char*)resPdu,resPdu->uiPDULen);
    if(!resPdu)
    {
        // 释放 resPdu
        free(resPdu);
        resPdu=NULL;
    }
}



// 接收消息的槽函数
void MyTcpSocket::recvMsg()
{
    // 读取PDU
    PDU* pdu = readPDU();
    // 处理请求
    PDU* resPdu = handleReq(pdu);

    qDebug()<<"recvMsg handleReq ret";

    // 响应为空，无需发送，函数返回
    if(!resPdu) return;
    // 发送响应
    sendPDU(resPdu);
    if(!pdu)
    {
        // 释放pdu
        free(pdu);
        pdu=NULL;
    }


}

// 处理下线的槽函数
void MyTcpSocket::clientOffline()
{
    // 将当前下线客户端的用户名 传给数据库进行下线操作
    OperateDB::getInstance().handleOffline(m_LoginName.toStdString().c_str());
    // 发送下线信号,将下线客户端的socket发送给TcpServer
    emit offline(this);

}
