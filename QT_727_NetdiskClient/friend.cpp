#include "friend.h"
#include "ui_friend.h"
#include "client.h"
#include "protocol.h"

#include <QInputDialog>
#include <QMessageBox>


Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
}

Friend::~Friend()
{
    delete ui;
}

// 查找用户的槽函数
void Friend::on_findUser_PB_clicked()
{
    // 输入框 获取 要查找的用户名
    QString strName = QInputDialog::getText(this,"查找用户","用户名：");
    if(strName.isEmpty())
    {
        QMessageBox::critical(this,"输入错误","用户名不能为空");
        return;
    }
    if(strName.toStdString().size() > 32)
    {
        QMessageBox::critical(this,"输入错误","用户名长度应小于32位");
        return;
    }
    if(strName==Client::getInstance().getLoginName())
    {
        QMessageBox::critical(this,"提示","查找的竟然是你自己！");
        return;
    }
    // 构建PDU对象
    PDU* pdu = initPDU(0);
    // 传入消息类型
    pdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_REQUEST;
    // 存放用户名
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.toStdString().size());

    // 发送消息，应该是socket对象调用write函数
    Client::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    // 释放pdu
    free(pdu);
    pdu = NULL;
}
