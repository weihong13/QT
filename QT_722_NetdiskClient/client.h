#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include "protocol.h"

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
    // 定义静态成员函数 获取单例对象
    static Client& getInstance();

    // 添加消息框和发送按钮的控件
    void set_Control();
public slots:
    // 客户端发送消息的槽函数
    void sendMsg();

private:
    Ui::Client *ui;
    // IP地址
    QString m_strIP;
    // 端口号
    quint16 m_uintPort;
    // 设置网络连接
    QTcpSocket m_tcpSocket;

    // 发送按钮
    QPushButton* sendBnt;
    QLineEdit* lineEdit;

    // 防止通过以下途径创建实例。
    // 私有化构造函数
    Client(QWidget *parent = nullptr);
    // 删除拷贝构造
    Client(const Client& instance) = delete;
    // 删除拷贝赋值运算符
    Client operator= (const Client&) =delete;
};
#endif // CLIENT_H
