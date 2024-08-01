#ifndef FRIEND_H
#define FRIEND_H

#include <onlineuser.h>
#include <QWidget>


namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();

    // 在线用户的界面是属于 friend界面的
    // 因此要添加一个OnlineUser类 成员变量
    OnlineUser* m_onlineUser;


private slots:
    void on_findUser_PB_clicked();

    void on_onlineUser_PB_clicked();

private:
    Ui::Friend *ui;
};

#endif // FRIEND_H
