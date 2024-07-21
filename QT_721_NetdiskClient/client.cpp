#include "client.h"
#include "ui_client.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QHostAddress>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    // 加载配置文件
    loadConfig();

    // 使用tcpSocket 进行连接
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_uintPort);

    // 连接成功后，会返回一个连接成功的信号，使用信号槽接收，并显示连接成功
    connect(&m_tcpSocket,&QTcpSocket::connected,this,&Client::showConnected);

}


Client::~Client()
{
    delete ui;
}

// 加载配置文件
void Client::loadConfig()
{
    // 获取文件路径
    QFile file(":/client.config");
    // 只读打开
    if(!file.open(QIODevice::ReadOnly))
    {
        // 打开失败提示错误
        QMessageBox::critical(this,"打开配置文件","打开配置文件失败");
        return;
    }
    // 读取配置文件信息
    QByteArray baData = file.readAll();
    // 测试--打印读取到的信息
    // qDebug()<<"baData" <<baData;

    // 将QByteArray 强转为字符串
    QString strData = QString(baData);
    // 将字符串按照 “\r\n” 进行切分
    QStringList strList = strData.split("\r\n");

    // 读取IP
    m_strIP = strList.at(0);
    // 读取端口号--端口号是2个字节16位的short类型，范围是0~65535
    m_uintPort = strList.at(1).toUShort();
    // 测试--分别打印IP和端口号
    // qDebug()<<"strIP: "<<m_strIP;
    // qDebug()<<"uintPort: "<<m_uintPort;

    // 关闭文件
    file.close();
}

// 连接成功展示
void Client::showConnected()
{
    // 消息对话框，展示连接成功。
    QMessageBox* msgBox = new QMessageBox;
    msgBox->information(this,"建立连接","连接成功");
}

// 获取网络连接
QTcpSocket& Client::getTcpSocket()
{
    return m_tcpSocket;
}

// 实现静态成员函数 获取单例对象
Client &Client::getInstance()
{
    // 返回静态局部引用对象，防止调用拷贝构造，静态变量只能赋值一次
    // 在C++11之后，默认支持线程安全
    static Client instance;
    return instance;
}
