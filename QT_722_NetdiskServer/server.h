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
    Server(QWidget *parent = nullptr);
    ~Server();

    // 加载本地文件
    void loadConfig();

private:
    Ui::Server *ui;
    QString m_IP;
    quint16 m_Port;
};
#endif // SERVER_H
