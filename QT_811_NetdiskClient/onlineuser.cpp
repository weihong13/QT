#include "onlineuser.h"
#include "ui_onlineuser.h"
#include "client.h"

#include <QMessageBox>

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
    // 主窗口关闭，在线用户窗口也会跟着关闭
    setAttribute(Qt::WA_QuitOnClose,false);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

// 展示在线用户
void OnlineUser::showOnlineUser(QStringList nameList)
{
    ui->onlineUser_LW->clear();
    ui->onlineUser_LW->addItems(nameList);
}

void OnlineUser::on_onlineUser_LW_itemDoubleClicked(QListWidgetItem *item)
{
    // 获取目标客户端的用户名
    QString strTarName = item->text();
    // 添加一个询问框，添加时进行确认
    int ret =  QMessageBox::question(this,"添加好友",QString("是否添加用户: %1 为好友？").arg(strTarName));
    if(ret == QMessageBox::Yes)
    {
        // 测试
        qDebug()<<"on_onlineUser_LW_itemDoubleClicked QMessageBox::Yes";
        // 要添加好友，得到当前客户端的用户名
        QString strCurName =  Client::getInstance().getLoginName();

        // 测试
        qDebug()<<"on_onlineUser_LW_itemDoubleClicked strCurName:"<<strCurName;
        qDebug()<<"on_onlineUser_LW_itemDoubleClicked strTarName:"<<strTarName;
        // 初始化pdu
        PDU* pdu = initPDU(0);
        // 添加消息类型为 添加好友请求
        pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        // 添加数据
        memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
        memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
        // 测试--打印检测发送的内容
        qDebug()<<"on_onlineUser_LW_itemDoubleClicked uiMsgType: "<<pdu->uiMsgType;
        qDebug()<<"on_onlineUser_LW_itemDoubleClicked strCurName: "<<pdu->caData;
        qDebug()<<"on_onlineUser_LW_itemDoubleClicked strTarName: "<<pdu->caData+32;

        // 发送消息
        Client::getInstance().sendPDU(pdu);

    }
    else
    {
        // 不添加好友，直接返回
        return;
    }



}
