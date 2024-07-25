#include "mytcpsocket.h"
#include "operatedb.h"

MyTcpSocket::MyTcpSocket()
{
    // 将socket中，接收到信息触发的信号，与取出信息的信号槽函数进行关联
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);
}

void MyTcpSocket::recvMsg()
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
//    qDebug()<<"recvMsg 结构体总长度："<<pdu->uiPDULen;
//    qDebug()<<"recvMsg 消息类型："<<pdu->uiMsgType;
//    qDebug()<<"recvMsg 消息长度："<<pdu->uiMsgLen;
//    qDebug()<<"recvMsg 参数1："<<pdu->caData;
//    qDebug()<<"recvMsg 参数2："<<pdu->caData+32;
//    qDebug()<<"recvMsg 接收到的消息："<<pdu->caMsg;

    // 根据消息类型对消息进行处理
    switch (pdu->uiMsgType)
    {
        // 注册请求
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            // 将消息取出
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            memcpy(caName,pdu->caData,32);
            memcpy(caPwd,pdu->caData+32,32);
            // 测试
            qDebug()<<"recvMsg REGIST caName: "<<caName;
            qDebug()<<"recvMsg REGIST caPwd: "<<caPwd;

            // 处理消息
            bool ret = OperateDB::getInstance().handleRegist(caName,caPwd);

            // 向客户端发送响应
            // 初始化响应注册的PDU对象
            PDU* registPdu = initPDU(0);
            // 消息类型为注册响应
            registPdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            // 将消息存储到消息结构体
            memcpy(registPdu->caData,&ret,sizeof(bool));
            // 利用socket 向客户端发送 注册的响应
            write((char*)registPdu,registPdu->uiPDULen);

            break;
        }
        // 登录请求
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            // 将消息取出
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            memcpy(caName,pdu->caData,32);
            memcpy(caPwd,pdu->caData+32,32);
            // 测试
            qDebug()<<"recvMsg LOGIN caName: "<<caName;
            qDebug()<<"recvMsg LOGIN caPwd: "<<caPwd;

            // 处理消息
            bool ret = OperateDB::getInstance().handleLogin(caName,caPwd);
            if(ret)
            {
                m_userName = caName;
            }
            // 向客户端发送响应
            // 初始化响应注册的PDU对象
            PDU* loginPdu = initPDU(0);
            // 消息类型为注册响应
            loginPdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            // 将消息存储到消息结构体
            memcpy(loginPdu->caData,&ret,sizeof(bool));
            // 利用socket 向客户端发送 注册的响应
            write((char*)loginPdu,loginPdu->uiPDULen);

            break;
        }

        default:
            break;
    }

    // 释放pdu
    free(pdu);
    pdu=NULL;

}
