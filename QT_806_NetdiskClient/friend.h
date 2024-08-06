#ifndef FRIEND_H
#define FRIEND_H



#include <onlineuser.h>
#include <QWidget>
#include "chat.h"

namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();
    // 发送刷新好友的请求
    void flushFriendReq();

    void showFirend(QStringList nameList);

    // 在线用户的界面是属于 friend界面的
    // 因此要添加一个OnlineUser类 成员变量
    OnlineUser* m_onlineUser;

    // 好友聊天
    Chat* m_chat;


private slots:
    void on_findUser_PB_clicked();

    void on_onlineUser_PB_clicked();

    void on_flushFriend_PB_clicked();

    void on_delFriend_PB_clicked();


    void on_friend_LW_itemDoubleClicked(QListWidgetItem *item);

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
};

#endif // FRIEND_H
