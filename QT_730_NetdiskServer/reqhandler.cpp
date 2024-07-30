#include "operatedb.h"
#include "reqhandler.h"


#include <QDebug>

ReqHandler::ReqHandler(QObject *parent) : QObject(parent)
{

}

PDU *ReqHandler::regist()
{
    // 将消息取出
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,m_pdu->caData,32);
    memcpy(caPwd,m_pdu->caData+32,32);
    // 测试
    qDebug()<<"ReqHandler regist caName: "<<caName;
    qDebug()<<"ReqHandler regist caPwd: "<<caPwd;

    // 处理消息
    bool ret = OperateDB::getInstance().handleRegist(caName,caPwd);

    // 向客户端发送响应
    // 初始化响应注册的PDU对象
    PDU* registPdu = initPDU(0);
    // 消息类型为注册响应
    registPdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    // 将消息存储到消息结构体
    memcpy(registPdu->caData,&ret,sizeof(bool));

    return registPdu;
}

PDU *ReqHandler::login(QString& loginName)
{
    // 将消息取出
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,m_pdu->caData,32);
    memcpy(caPwd,m_pdu->caData+32,32);
    // 测试
    qDebug()<<"ReqHandler login caName: "<<caName;
    qDebug()<<"ReqHandler login caPwd: "<<caPwd;

    // 处理消息
    bool ret = OperateDB::getInstance().handleLogin(caName,caPwd);
    if(ret)
    {
        // 登录成功，记录登陆成功的用户名
        loginName = caName;
    }
    // 向客户端发送响应
    // 初始化响应登录的PDU对象
    PDU* loginPdu = initPDU(0);
    // 消息类型为注册响应
    loginPdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    // 将消息存储到消息结构体
    memcpy(loginPdu->caData,&ret,sizeof(bool));
    memcpy(loginPdu->caData+32,caName,32);

    return loginPdu;
}

PDU *ReqHandler::findUser(PDU *pdu)
{
    // 将消息取出
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    // 测试
    qDebug()<<"ReqHandler findUser caName: "<<caName;

    // 处理消息
    int ret = OperateDB::getInstance().handleFindUser(caName);
    // 向客户端发送响应
    // 初始化响应查找用户的PDU对象
    PDU* findUserPdu = initPDU(0);
    // 消息类型为查找用户响应
    findUserPdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_RESPOND;
    // 将消息存储到消息结构体
    memcpy(findUserPdu->caData,&ret,sizeof(int));

    return findUserPdu;
}

PDU *ReqHandler::onlineUser()
{
    // 处理消息
    QStringList nameList = OperateDB::getInstance().handleOnlineUser();

    // 获取列表大小
    int listSize = nameList.size();
    uint uiMsgLen = listSize*32;

    // 向客户端发送响应
    // 初始化响应在线用户的PDU对象
    PDU* onlineUserPdu = initPDU(uiMsgLen);
    // 消息类型为在线用户响应
    onlineUserPdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USER_RESPOND;


    // 将用户名 挨个放到 caMsg中
    for(int i = 0; i<listSize; i++)
    {
        // 测试
        // qDebug()<<"ReqHandler  onlineUser " << nameList.at(i);

        // 将每一个用户名都存储到 caMsg中
        memcpy(onlineUserPdu->caMsg+i*32,nameList.at(i).toStdString().c_str(),32);
    }

    return onlineUserPdu;
}
