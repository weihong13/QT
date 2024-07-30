#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
    // 主窗口关闭，在线用户窗口也会跟着关闭
    setAttribute(Qt::WA_QuitOnClose,false);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

// 展示在线用户
void OnlineUser::showOnlineUser(QStringList nameList)
{
    ui->onlineUser_LW->clear();
    ui->onlineUser_LW->addItems(nameList);

}
