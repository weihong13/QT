#include "index.h"
#include "ui_index.h"

Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);
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

Friend* Index::getFriend()
{
    return ui->friendPage;
}

File *Index::getFile()
{
    return ui->filePage;
}

// 首页点击好友按钮，切换到好友页面
void Index::on_friend_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->friend_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgb(255, 255, 255);}");
    ui->file_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgba(255, 255, 255,0);}");
}

// 首页点击文件按钮，切换到文件页面
void Index::on_file_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->file_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgb(255, 255, 255);}");
    ui->friend_PB->setStyleSheet("QPushButton{padding:25px;background-color: rgba(255, 255, 255,0);}");
}
