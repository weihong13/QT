#include "reshandler.h"
#include "client.h"
#include "index.h"

#include <QMessageBox>

ResHandler::ResHandler(QObject *parent) : QObject(parent)
{

}

void ResHandler::regist()
{
    // 将消息取出
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    // 根据返回的响应进行处理
    if(ret)
    {
        QMessageBox::information(&Client::getInstance(),"注册","注册成功");
    }
    else
    {
        QMessageBox::information(&Client::getInstance(),"注册","注册失败：用户名或密码非法");
    }
}

void ResHandler::login(QString &loginName)
{
    // 将消息取出
    bool ret;
    char caName[32] = {'\0'};
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    memcpy(caName,m_pdu->caData+32,32);
    // 根据返回的响应进行处理
    if(ret)
    {
        loginName = caName;
        qDebug()<<"recvMsg LOGIN_REQUEST m_userName"<<loginName;
        QMessageBox::information(&Client::getInstance(),"登录","登录成功");
        // 登录成功后，跳转到首页
        Index::getInstance().show();
        // 隐藏登录界面
        Client::getInstance().hide();
    }
    else
    {
        QMessageBox::information(&Client::getInstance(),"登录","登录失败：用户名或密码非法");
    }
}

void ResHandler::findUser()
{
    // 将消息取出
    int ret;
    memcpy(&ret,m_pdu->caData,sizeof(int));
    // 根据返回的响应进行处理
    if(ret == -1)
    {
        QMessageBox::information(&Index::getInstance(),"查找用户","该用户不存在");
        return;
    }
    else if(ret == 0)
    {
        QMessageBox::information(&Index::getInstance(),"查找用户","该用户不在线");
        return;
    }
    else if(ret == 1)
    {
        QMessageBox::information(&Index::getInstance(),"查找用户","该用户在线");
        return;
    }
    else
    {
        QMessageBox::critical(&Index::getInstance(),"查找用户","查找错误");
        return;
    }

}

void ResHandler::onlineUser(QString &loginName)
{
    // 获取在线用户的个数
    uint listSize = m_pdu->uiMsgLen/32;
    qDebug()<<"listSize  "<<listSize;
    // 创建变量存储在线用户的用户名
    char userName[32];
    QStringList nameList;
    // 将caMsg中的用户名挨个取出，并放到nameList中
    for(uint i = 0; i <listSize; i++)
    {
        // 挨个取出用户名
        memcpy(userName,m_pdu->caMsg+i*32,32);
        // 测试
        // qDebug()<<"ONLINE_USER_RESPOND  "<<QString(userName);
        // 跳过自己
        if(QString(userName) == loginName)
        {
            continue;
        }
        // 将取到的用户名存放到 nameList中
        nameList.append(QString(userName));
    }
    // 调用展示在线用户的函数
    Index::getInstance().getFriend()->m_onlineUser->showOnlineUser(nameList);
}
