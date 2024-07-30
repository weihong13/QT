#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include "protocol.h"
#include "reshandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:

    ~Client();
    // 加载配置文件
    void loadConfig();
    // 槽函数-连接成功展示
    void showConnected();
    // 获取网络连接
    QTcpSocket& getTcpSocket();
    // 获取当前登录的用户名
    QString& getLoginName();

    // 读出PDU
    PDU* readPDU();
    // 处理响应
    void handleRes(PDU* pdu);

    // 定义静态成员函数 获取单例对象
    static Client& getInstance();

public slots:
    // 客户端发送消息的槽函数
    // void sendMsg();

    // 接收消息
    void recvMsg();

private slots:
    void on_regist_PB_clicked();

    void on_login_PB_clicked();

private:
    Ui::Client *ui;
    // IP地址
    QString m_strIP;
    // 端口号
    quint16 m_uintPort;
    // 设置网络连接
    QTcpSocket m_tcpSocket;
    // 登陆成功的用户名
    QString m_LoginName;
    // 响应处理器
    ResHandler* m_rh;





    // 防止通过以下途径创建实例。
    // 私有化构造函数
    Client(QWidget *parent = nullptr);
    // 删除拷贝构造
    Client(const Client& instance) = delete;
    // 删除拷贝赋值运算符
    Client operator= (const Client&) =delete;
};
#endif // CLIENT_H
