#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include "mytcpserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:

    ~Server();

    static Server& getInstance();

    // 加载本地文件
    void loadConfig();
    // 获取路径
    QString getRootPath();

private:
    Ui::Server *ui;
    QString m_IP;
    quint16 m_Port;
    QString m_RootPath;

    Server(QWidget *parent = nullptr);
};
#endif // SERVER_H
