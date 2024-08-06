#include "index.h"
#include "ui_index.h"
#include "client.h"

Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);
    this->setWindowTitle(Client::getInstance().getLoginName());
}

Index::~Index()
{
    delete ui;
}

Index &Index::getInstance()
{
    static Index instance;
    return instance;
}
// 获取好友界面
Friend* Index::getFriend()
{
    return ui->friendPage;
}
// 获取文件界面
File *Index::getFile()
{
    return ui->filePage;
}

// 首页点击好友按钮，切换到好友页面
void Index::on_friend_PB_clicked()
{
    // 设置切换的界面--好友界面
    ui->stackedWidget->setCurrentIndex(0);
    // 设置按钮的样式
    ui->friend_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgb(255, 255, 255);}");
    ui->file_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgba(255, 255, 255,0);}");
}

// 首页点击文件按钮，切换到文件页面
void Index::on_file_PB_clicked()
{
    // 设置切换的界面--文件界面
    ui->stackedWidget->setCurrentIndex(1);
    // 设置按钮的样式
    ui->file_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgb(255, 255, 255);}");
    ui->friend_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgba(255, 255, 255,0);}");
}
