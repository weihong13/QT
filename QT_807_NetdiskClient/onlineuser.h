#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class OnlineUser;
}

class OnlineUser : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUser(QWidget *parent = nullptr);
    ~OnlineUser();

    // 展示在线用户
    void showOnlineUser(QStringList nameList);


private slots:
    // 双击在线用户列表，申请添加好友
    void on_onlineUser_LW_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H
