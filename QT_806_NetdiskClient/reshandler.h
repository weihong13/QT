#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"

#include <QObject>

class ResHandler : public QObject
{
    Q_OBJECT
public:
    explicit ResHandler(QObject *parent = nullptr);
    // 处理注册响应
    void regist();
    // 处理登录响应
    void login(QString& loginName);
    // 处理查找用户响应
    void findUser();
    // 处理在线用户
    void onlineUser(QString &loginName);

    // 处理添加好友的响应
    void addFriendRes();

    // 处理其他客户发来的添加好友请求
    void addFriendReq();


    // 刷新好友
    void flushFriend();

    // 删除好友
    void  deleteFriend();

    // 好友聊天请求
    void friendChat();

    // 创建文件夹
    void mkdir();

    // 刷新文件
    void flushFile();

    PDU* m_pdu;
signals:

};

#endif // RESHANDLER_H
