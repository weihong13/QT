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

    // 添加消息框和发送按钮的控件
    set_Control();
    // 将发送按钮与发送消息的槽函数进行关联
    connect(sendBnt,&QPushButton::clicked,this,&Client::sendMsg);


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

// 添加消息框和发送按钮的控件
void Client::set_Control()
{
    this->lineEdit = new QLineEdit(this);
    lineEdit->resize(500,30);
    lineEdit->move(15,20);
    lineEdit->show();

    this->sendBnt = new QPushButton(this);
    sendBnt->move(515,20);
    sendBnt->setText("发送");
    sendBnt->show();
}

// 发送消息的槽函数
void Client::sendMsg()
{
   // 读取 lineEdit 中的内容
   QString strMsg = this->lineEdit->text();
   if(strMsg.isEmpty()) // 如果当前消息为空，直接返回
   {
       return;
   }

   // 读到消息，获取消息的长度，得先将消息转换为标准的字符串，以便求出长度
   // 不转的话，中文是没法读全的（一个汉字占好几个字符）
   uint uiMsgLen = strMsg.toStdString().size();
   // 初始化PDU，传入的消息长度加一，多加一个消息结束标志
   PDU* pdu = initPDU(uiMsgLen+1);
   // 利用函数，将读到消息，存放在PDU中的柔性数组中。
   // 这里需要按照 memcpy的参数形式，将前两个参数的类型修改为 char*
   memcpy((char*)pdu->caMsg,strMsg.toStdString().c_str(),uiMsgLen);
   // 给定消息类型，这里随便给一个就行
   pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;

   // 测试。查看消息长度，和消息内容是否正确
   qDebug()<<"uiMsgLen: "<<pdu->uiMsgLen;
   qDebug()<<"caMsg: "<<pdu->caMsg;

   // 利用socket发送消息。
   m_tcpSocket.write((char*)pdu,pdu->uiPDULen);

   // 完成消息，释放pdu
   free(pdu);
   pdu = NULL;

}
