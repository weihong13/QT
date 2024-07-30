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

    PDU* m_pdu;
signals:

};

#endif // RESHANDLER_H
