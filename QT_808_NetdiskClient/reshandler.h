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
    // 处理在线用户响应
    void onlineUser(QString &loginName);

    // 处理添加好友的响应
    void addFriendRes();

    // 处理其他客户发来的添加好友请求
    void addFriendReq();


    // 刷新好友响应
    void flushFriend();

    // 删除好友响应
    void  deleteFriend();

    // 好友聊天请求响应
    void friendChat();

    // 创建文件夹响应
    void mkdir();

    // 刷新文件响应
    void flushFile();

    // 删除文件夹响应
    void rmdir();

    // 删除文件响应
    void rmFile();

    PDU* m_pdu;
signals:

};

#endif // RESHANDLER_H
