#include "client.h"
#include "index.h"
#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"

#include <QMessageBox>

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::showFriend_LW()
{
    ui->friend_LW->clear();
    QListWidget* friend_LW = Index::getInstance().getFriend()->getFriend_LW();
    for(int i = 0; i < friend_LW->count(); i++)
    {
        QListWidgetItem* pItem = friend_LW->item(i);
        QListWidgetItem* newItem = new QListWidgetItem(*pItem);
        ui->friend_LW->addItem(newItem);
    }

}


// 全选
void ShareFile::on_selectAll_PB_clicked()
{
    for(int i = 0; i < ui->friend_LW->count(); i++)
    {
        ui->friend_LW->item(i)->setSelected(true);
    }
}

// 取消全选
void ShareFile::on_cancelSelectAll_PB_clicked()
{
    for(int i = 0; i < ui->friend_LW->count(); i++)
    {
        ui->friend_LW->item(i)->setSelected(false);
    }
}

// 确认
void ShareFile::on_ok_PB_clicked()
{
    QList<QListWidgetItem*> pItems =  ui->friend_LW->selectedItems();
    // 计算选择的好友个数
    int friendNum = pItems.size();
    // 没选择好友，提示
    if(friendNum == 0)
    {
        QMessageBox::information(this,"分享文件","请选择要分享的好友");
        return;
    }
    // 获得当前登录的用户名
    QString strLoginName = Client::getInstance().getLoginName();
    // 获得分享文件的完整路径
    QString strShareFilePath = m_sharPath + '/' + m_shareFileName;
    // 构建pdu
    PDU* pdu = initPDU(friendNum*32 + strShareFilePath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    // 存入当前登录的用户名 以及 好友数量
    memcpy(pdu->caData,strLoginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&friendNum,sizeof (int));
    // 将要分享的好友用户名 挨个存入caMsg
    for(int i=0; i<friendNum; i++)
    {
        memcpy(pdu->caMsg+i*32,pItems.at(i)->text().toStdString().c_str(),32);
    }
    // 将分享文件的完整路径 存入caMsg
    memcpy(pdu->caMsg+friendNum*32,strShareFilePath.toStdString().c_str(),strShareFilePath.toStdString().size());
    // 发送消息
    Client::getInstance().sendPDU(pdu);


}
