#ifndef REQHANDLER_H
#define REQHANDLER_H

#include "protocol.h"

#include <QObject>

class ReqHandler : public QObject
{
    Q_OBJECT
public:
    explicit ReqHandler(QObject *parent = nullptr);
    PDU* regist();
    PDU* login(QString& loginName);
    PDU* findUser();
    PDU* onlineUser();

    PDU* m_pdu;


signals:

};

#endif // REQHANDLER_H
