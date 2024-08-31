#ifndef CLIENTTASK_H
#define CLIENTTASK_H

#include <QRunnable>
#include <QObject>
#include "mytcpsocket.h"

class ClientTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ClientTask();
    ClientTask(MyTcpSocket *socket);
    MyTcpSocket *m_socket;
    void run() override;

signals:

};

#endif // CLIENTTASK_H
