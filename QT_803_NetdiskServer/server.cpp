#include "server.h"
#include "ui_server.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    // 加载配置文件
    loadConfig();

    // 进行监听
    MyTcpServer::getInstance().listen(QHostAddress(m_IP),m_Port);

}

Server::~Server()
{
    delete ui;
}

// 加载配置文件
void Server::loadConfig()
{
    // 获取文件路径
    QFile file(":/server.config");
    // 打开文件
    if(!file.open(QIODevice::ReadOnly))
    {   // 打开失败--错误提示
        QMessageBox::critical(this,"打开配置文件","打开配置文件失败");
        return;
    }
    // 读取文件内容
    QByteArray baData = file.readAll();
    // 转换格式为 QString
    QString strData = QString(baData);
    // 将字符串按照 "\r\n" 进行分割
    QStringList strList = strData.split("\r\n");
    // 获取IP地址
    this->m_IP = strList.at(0);
    // 获取端口号
    this->m_Port = strList.at(1).toShort();
    // 测试打印
    // qDebug()<<"IP："<<m_IP;
    // qDebug()<<"Port："<<m_Port;

}

