#include "client.h"
#include "ui_client.h"
#include "index.h"

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
    // 创建一个 响应处理器对象
    m_rh = new ResHandler;
    // 加载配置文件
    loadConfig();

    // 使用tcpSocket 进行连接
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_uintPort);

    // 连接成功后，会返回一个连接成功的信号，使用信号槽接收，并显示连接成功
    connect(&m_tcpSocket,&QTcpSocket::connected,this,&Client::showConnected);


    // 将socket中，接收到信息触发的信号，与取出信息的信号槽函数进行关联
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&Client::recvMsg);
}

// 析构
Client::~Client()
{
    if(!m_rh) delete m_rh;
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

    m_RootPath = strList.at(2);
    // 测试--分别打印IP和端口号
    // qDebug()<<"loadConfig strIP: "<<m_strIP;
    // qDebug()<<"loadConfig uintPort: "<<m_uintPort;

    qDebug()<<"loadConfig m_RootPath: "<<m_RootPath;

    // 关闭文件
    file.close();
}

// 连接成功展示
void Client::showConnected()
{
    qDebug()<<"已成功连接服务器！";
}

// 获取网络连接
QTcpSocket& Client::getTcpSocket()
{
    return m_tcpSocket;
}

QString &Client::getLoginName()
{
    return m_LoginName;
}

QString Client::getRootPath()
{
    return m_RootPath;
}



// 实现静态成员函数 获取单例对象
Client &Client::getInstance()
{

    // 返回静态局部引用对象，防止调用拷贝构造，静态变量只能赋值一次
    // 在C++11之后，默认支持线程安全
    static Client instance;
    return instance;
}
// 读出PDU
PDU *Client::readPDU()
{
    // 打印socket中的数据长度
    qDebug()<<"Client readPDU socket中接收到的数据长度："<<m_tcpSocket.bytesAvailable();

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

//    // 测试--打印输出消息结构体的内容
//    qDebug()<<"readPDU 结构体总长度："<<pdu->uiPDULen;
//    qDebug()<<"readPDU 消息类型："<<pdu->uiMsgType;
//    qDebug()<<"readPDU 消息长度："<<pdu->uiMsgLen;
    qDebug()<<"Client readPDU caData:"<<pdu->caData;
    qDebug()<<"Client readPDU caData+32:"<<pdu->caData+32;
    qDebug()<<"Client readPDU caMsg:"<<pdu->caMsg;

    return pdu;
}

void Client::sendPDU(PDU *pdu)
{
    qDebug()<<"Client sendPDU";
    // 利用socket 向客户端发送 注册的响应
    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    if(pdu)
    {
        // 释放 pdu
        free(pdu);
        pdu=NULL;
    }

}

// 处理响应
void Client::handleRes(PDU *pdu)
{
    m_rh->m_pdu = pdu;
    // 根据消息类型对消息进行处理
    switch (pdu->uiMsgType)
    {
        // 注册响应
        case ENUM_MSG_TYPE_REGIST_RESPOND:
        {
            m_rh->regist();
            break;
        }

        // 登录响应
        case ENUM_MSG_TYPE_LOGIN_RESPOND:
        {
            m_rh->login();
            break;
        }

        // 查找用户响应
        case ENUM_MSG_TYPE_FIND_USER_RESPOND:
        {
            m_rh->findUser();
            break;
        }

        // 在线用户响应
        case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:
        {
            m_rh->onlineUser(m_LoginName);
            break;
        }
        // 添加用户响应
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        {
            m_rh->addFriendRes();
            break;
        }
        // 添加用户请求
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            m_rh->addFriendReq();
            break;
        }
        // 刷新好友的响应
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
        {
            m_rh->flushFriend();
            break;
        }
        // 删除好友的响应
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        {
            m_rh->deleteFriend();
            break;
        }

        // 好友聊天的请求
        case ENUM_MSG_TYPE_FRIEND_CHAT_REQUEST:
        {
            m_rh->friendChat();
            break;
        }
        // 创建文件夹的响应
        case ENUM_MSG_TYPE_MKDIR_RESPOND:
        {
            m_rh->mkdir();
            break;
        }
        // 刷新文件夹的响应
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
        {
            m_rh->flushFile();
            break;
        }
        // 删除文件夹的响应
        case ENUM_MSG_TYPE_RMDIR_RESPOND:
        {
            m_rh->rmdir();
            break;
        }
        // 删除文件的响应
        case ENUM_MSG_TYPE_RMFILE_RESPOND:
        {
            m_rh->rmFile();
            break;
        }
        // 重命名文件的响应
        case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
        {
            m_rh->renameFile();
            break;
        }
        // 移动文件时的刷新文件夹的响应
        case ENUM_MSG_TYPE_MOVE_FLUSH_FILE_RESPOND:
        {
            m_rh->moveFlushFile();
            break;
        }
        // 移动文件时的刷新文件夹的响应
        case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
        {
            m_rh->moveFile();
            break;
        }
        // 上传文件的响应
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
        {
            m_rh->uploadFile();
            break;
        }
        // 上传文件数据的响应
        case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND:
        {
            m_rh->uploadFileData();
            break;
        }


        default:
            break;
    }
}

// 接收消息
void Client::recvMsg()
{
    // 打印socket中的数据长度
    qDebug()<<"socket中接收到的数据长度："<<m_tcpSocket.bytesAvailable();

    // 读取socket中的全部数据
    QByteArray data = m_tcpSocket.readAll();
    // 将data中的数据 添加到 成员变量中
    // 这里只能是添加，不能是赋值，赋值就无法解决半包的问题
    m_buff.append(data);

    while(m_buff.size() >= int(sizeof (PDU)))
    {
        PDU* pdu = (PDU*)m_buff.data();
        if(m_buff.size() < int(pdu->uiPDULen))
        {
            break;
        }
        // 测试--打印输出消息结构体的内容
        qDebug()<<"Client recvMsg 结构体总长度："<<pdu->uiPDULen;
        qDebug()<<"Client recvMsg 消息长度："<<pdu->uiMsgLen;
        qDebug()<<"Client recvMsg 消息类型："<<pdu->uiMsgType;
        qDebug()<<"Client recvMsg caData："<<pdu->caData;
        qDebug()<<"Client recvMsg caData+32："<<pdu->caData+32;
        qDebug()<<"Client recvMsg caMsg："<<pdu->caMsg;
        // 调用处理响应的函数
        handleRes(pdu);

        // 将读取完成的数据去除
        m_buff.remove(0,pdu->uiPDULen);
    }

}

// 注册按钮的槽函数
void Client::on_regist_PB_clicked()
{
    qDebug()<<"on_regist_PB_clicked";
    // 从ui对象中选取用户名和密码的输入框，并读出内容
    QString strName =  ui->userName_LE->text();
    QString strpwd =  ui->pwd_LE->text();
    // 判空，如果是空，直接返回
    if(strName.isEmpty()||strpwd.isEmpty()) return;
    // 检查输入内容长度，根据表的设计，数据长度应该小于32
    if(strName.toStdString().size()>32||strpwd.toStdString().size()>32)
    {
        QMessageBox::critical(this,"注册","用户名或密码长度应小于32");
        return;
    }
    // 检查密码长度，要求密码必须大于3位
    if(strpwd.toStdString().size()<3)
    {
        QMessageBox::critical(this,"注册","密码长度应大于3）");
        return;
    }

    // 初始化一个 PDU对象
    PDU* pdu = initPDU(0);
    // 输入消息类型为，注册请求
    pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
    // 将用户名和密码放入caData中，用户名防止前32位，密码放在后32位。
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strpwd.toStdString().c_str(),32);

    // 测试--打印检测发送的内容
    qDebug()<<"regist uiMsgType: "<<pdu->uiMsgType;
    qDebug()<<"regist strName: "<<pdu->caData;
    qDebug()<<"regist strpwd: "<<pdu->caData+32;

    // 通过socket将消息发送到服务器
    sendPDU(pdu);
}



// 登录按钮的槽函数
void Client::on_login_PB_clicked()
{
    // 从ui对象中选取 输入框，并读出内容
    QString strName =  ui->userName_LE->text();
    QString strpwd =  ui->pwd_LE->text();
    // 判空，如果是空，直接返回
    if(strName.isEmpty()||strpwd.isEmpty()) return;
    // 检查输入内容长度，根据表的设计，数据长度应该小于32
    if(strName.toStdString().size()>32||strpwd.toStdString().size()>32)
    {
        QMessageBox::critical(this,"登录","用户名或密码长度应小于32");
        return;
    }
    // 检查密码长度，要求密码必须大于3位
    if(strpwd.toStdString().size()<3)
    {
        QMessageBox::critical(this,"登录","密码长度应大于3）");
        return;
    }

    // 初始化一个 PDU对象
    PDU* pdu = initPDU(0);
    // 输入消息类型为，登录请求
    pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
    // 将用户名和密码放入caData中，用户名防止前32位，密码放在后32位。
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strpwd.toStdString().c_str(),32);
    // 测试--打印检测发送的内容
    qDebug()<<"login uiMsgType: "<<pdu->uiMsgType;
    qDebug()<<"login strName: "<<pdu->caData;
    qDebug()<<"login strpwd: "<<pdu->caData+32;

    // 通过socket将消息发送到服务器
    sendPDU(pdu);
}
