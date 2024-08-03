#include "operatedb.h"
#include "reqhandler.h"
#include "mytcpserver.h"


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

    qDebug()<<"ReqHandler registPdu ret: "<<registPdu->caData;

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

// 处理查找用户的请求
PDU *ReqHandler::findUser()
{
    // 将消息取出
    char caName[32] = {'\0'};
    memcpy(caName,m_pdu->caData,32);
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
    memcpy(findUserPdu->caData+32,caName,32);

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

// 处理添加好友的请求
PDU *ReqHandler::addFriend()
{
    // 取出用户名
    char curName[32] = {'\0'};
    char tarName[32] = {'\0'};
    memcpy(curName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    // 测试
    qDebug()<<"ReqHandler addFriend curName: "<<curName;
    qDebug()<<"ReqHandler addFriend tarName: "<<tarName;

    // 在数据库中判断当前客户端与目标客户端是否已经是好友，以及目标客户端是否在线
    int ret = OperateDB::getInstance().handleAddFriend(curName,tarName);

    // 根据数据库响应，决定是否要转发
    if(ret==1)
    {
        // 用户在线，向目标用户转发，是否同意添加好友的请求
        m_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST;
        MyTcpServer::getInstance().resend(tarName,m_pdu);
        return NULL;


    }
    else
    {
        // 如果添加过程出现错误，直接返回 添加好友的响应
        // 初始化响应查找用户的PDU对象
        PDU* resPdu = initPDU(0);
        // 消息类型为添加好友的响应
        resPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        // 将消息存储到消息结构体
        memcpy(resPdu->caData,&ret,sizeof(int));
        return resPdu;

    }

}

// 处理 是否同意添加好友的请求
void ReqHandler::addFriendAgree()
{
    // 取出用户名，以及目标用户的态度
    char curName[32] = {'\0'};
    char tarName[32] = {'\0'};
    int ret;
    memcpy(curName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    memcpy(&ret,m_pdu->caMsg,sizeof(int));

    // 测试--打印检测发送的内容
    qDebug()<<"ResHandler addFriendAgree uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler addFriendAgree curName: "<<m_pdu->caData;
    qDebug()<<"ResHandler addFriendAgree tarName: "<<m_pdu->caData+32;
    qDebug()<<"ResHandler addFriendAgree ret : "<<m_pdu->caMsg;
    if(ret==1)
    {
        // 目标用户同意添加好友
        qDebug()<<"ResHandler addFriendAgree ret 1 : ";
        // 建立好友关系
        OperateDB::getInstance().handleAddFriendAgree(curName,tarName);
        // 用户在线，转发添加请求
        m_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND;
        MyTcpServer::getInstance().resend(curName,m_pdu);

    }
    else
    {
        // 目标用户拒绝添加好友
        qDebug()<<"ResHandler addFriendAgree ret 2 : ";
        m_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND;
        MyTcpServer::getInstance().resend(curName,m_pdu);
    }
}
// 刷新好友列表请求
PDU *ReqHandler::flushFriend(QString& curName)
{
    // 处理消息
    QStringList friendList = OperateDB::getInstance().handleFlushFriend(curName);

    // 获取好友列表大小
    int listSize = friendList.size();
    uint uiMsgLen = listSize*32;

    // 向客户端发送响应
    // 初始化响应刷新好友的PDU对象
    PDU* flushFriendPdu = initPDU(uiMsgLen);
    // 消息类型为刷新好友的响应
    flushFriendPdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;


    // 将用户名 挨个放到 caMsg中
    for(int i = 0; i<listSize; i++)
    {
        // 测试
        qDebug()<<"ReqHandler  flushFriend " << friendList.at(i);

        // 将每一个用户名都存储到 caMsg中
        memcpy(flushFriendPdu->caMsg+i*32,friendList.at(i).toStdString().c_str(),32);
    }

    return flushFriendPdu;
}

PDU *ReqHandler::deleteFriend(QString &curName)
{
    // 取出要删除的用户名
    char tarName[32] = {'\0'};
    memcpy(tarName,m_pdu->caData,32);

    // 测试--打印检测发送的内容
    qDebug()<<"ResHandler deleteFriend uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler deleteFriend curName: "<<m_pdu->caData;
    // 处理消息，获取返回值
    int ret = OperateDB::getInstance().handleDeleteFriend(curName,tarName);
    // 构建响应PDU
    PDU* resPdu = initPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    memcpy(resPdu->caData,&ret,sizeof(int));

    return resPdu;
}

// 好友聊天--用于向目标客户端转发聊天内容
void ReqHandler::friendChat()
{
    // 取出目标用户名
    char tarName[32] = {'\0'};

    memcpy(tarName,m_pdu->caData+32,32);

    // 测试--打印检测发送的内容
    qDebug()<<"ResHandler friendChat uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler friendChat curName: "<<m_pdu->caData;
    qDebug()<<"ResHandler friendChat tarName: "<<m_pdu->caData+32;
    qDebug()<<"ResHandler friendChat caMsg : "<<m_pdu->caMsg;

    // 将消息转发给目标用户
    MyTcpServer::getInstance().resend(tarName,m_pdu);


}
