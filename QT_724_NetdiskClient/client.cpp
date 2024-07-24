#include "client.h"
#include "ui_client.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QHostAddress>
#include <string.h>

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


    // 将socket中，接收到信息触发的信号，与取出信息的信号槽函数进行关联
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&Client::recvMsg);
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


// 接收消息
void Client::recvMsg()
{
    // 打印socket中的数据长度
    qDebug()<<"socket中接收到的数据长度："<<m_tcpSocket.bytesAvailable();

    // 读出消息结构体的总长度，这部分内容会从socket中读出去
    uint uiPDULen = 0; // 传出参数
    // 读取的大小为 消息结构体总长度的数据类型大小
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));

    // 计算实际消息长度--柔性数组长度(消息结构体总长度-结构体大小)
    uint uiMsgLen = uiPDULen-sizeof(PDU);
    // 初始化一个PUD结构体，用于存储
    PDU* pdu = initPDU(uiMsgLen);

    // 读出socket中剩余的数据，消息结构体的总长度已经被读出去了，剩下的不是一个完整的PDU结构体。
    // 因此需要对pdu这个指针进行偏移，偏移的长度就是已经读出的数据长度（消息结构体总长度的数据类型大小）
    // 而剩余需要读出的内容为，消息结构体总长度 减去 消息结构体总长度的数据类型大小
    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

    // 测试--打印输出消息结构体的内容
    qDebug()<<"结构体总长度："<<pdu->uiPDULen;
    qDebug()<<"消息类型："<<pdu->uiMsgType;
    qDebug()<<"消息长度："<<pdu->uiMsgLen;
    qDebug()<<"参数1："<<pdu->caData;
    qDebug()<<"参数2："<<pdu->caData+32;
    qDebug()<<"接收到的消息："<<pdu->caMsg;

    // 释放pdu
    free(pdu);
    pdu=NULL;

}
