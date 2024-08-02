#ifndef REQHANDLER_H
#define REQHANDLER_H

#include "protocol.h"

#include <QObject>

class ReqHandler : public QObject
{
    Q_OBJECT
public:
    explicit ReqHandler(QObject *parent = nullptr);
    // 处理注册请求
    PDU* regist();
    // 处理登录请求
    PDU* login(QString& loginName);
    // 处理查找用户请求
    PDU* findUser();
    // 处理在线用户请求
    PDU* onlineUser();

    // 处理添加好友请求
    PDU* addFriend();

    // 处理同意添加好友请求
    void addFriendAgree();

    // 刷新好友列表请求
    PDU* flushFriend(QString& curName);
    // 删除好友请求
    PDU* deleteFriend(QString& curName);

    // 好友聊天
    void friendChat();


    PDU* m_pdu;


signals:

};

#endif // REQHANDLER_H
