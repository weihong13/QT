#include "friend.h"
#include "ui_friend.h"
#include "client.h"
#include "protocol.h"
#include "index.h"


#include <QInputDialog>
#include <QMessageBox>


Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    // 新建一个 OnlineUser 对象，用于展示在线用户的界面
    m_onlineUser = new OnlineUser;
    // 创建一个 Chat的对象，用于展示聊天界面
    m_chat = new Chat;
}

Friend::~Friend()
{
    if(!m_onlineUser) delete m_onlineUser;
    if(!m_chat) delete m_chat;
    delete ui;
}


void Friend::showFirend(QStringList nameList)
{
    ui->friend_LW->clear();
    ui->friend_LW->addItems(nameList);

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

    // 发送消息
    Client::getInstance().sendPDU(pdu);

}

void Friend::on_onlineUser_PB_clicked()
{
    // 如果该界面是隐藏的，则进行展示
    if(m_onlineUser->isHidden())
    {
       m_onlineUser->show();

       PDU* pdu = initPDU(0);
       pdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USER_REQUEST;
       // 发送消息
       Client::getInstance().sendPDU(pdu);
    }
}

// 发送刷新好友的请求
void Friend::flushFriendReq()
{
    qDebug()<<"Friend flushFriendReq";
    PDU* pdu = initPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    // 发送消息
    Client::getInstance().sendPDU(pdu);
}


void Friend::on_flushFriend_PB_clicked()
{
    qDebug()<<"Friend on_flushFriend_PB_clicked";
    flushFriendReq();
}

// 删除好友的按钮
void Friend::on_delFriend_PB_clicked()
{
   QListWidgetItem* pItem =  ui->friend_LW->currentItem();
   if(!pItem)
   {
       QMessageBox::information(this,"删除好友","请选择你要删除的好友");
       return;
   }
   QString tarName = pItem->text();

   int ret = QMessageBox::critical(this,"删除好友",QString("是否要删除 %1 ?").arg(tarName),"确认删除","取消");
   if(ret == 0)
   {
       qDebug()<<"确认删除";
       PDU* pdu = initPDU(0);
       memcpy(pdu->caData,tarName.toStdString().c_str(),32);
       pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
       // 发送消息
       Client::getInstance().sendPDU(pdu);
   }
   else
   {
       qDebug()<<"取消";
   }

}



// 双击打开聊天界面
void Friend::on_friend_LW_itemDoubleClicked(QListWidgetItem *item)
{
    // 添加一个询问框，添加时进行确认
    QMessageBox::StandardButton ret =  QMessageBox::question(this,"好友聊天","是否要与该好友聊天？",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    if(ret == QMessageBox::Yes)
    {
        // 测试
        qDebug()<<"on_friend_LW_itemDoubleClicked QMessageBox::Yes";
        // 要聊天，得到当前客户端的用户名和目标用户的用户名
        m_chat->m_curName = Client::getInstance().getLoginName();
        m_chat->m_tarName = item->text();
        // 如果该界面是隐藏的，则进行展示
        if(m_chat->isHidden())
        {
            qDebug()<<"打开聊天界面";
            // 设置聊天框的标题为 目标用户用户名
            m_chat->setWindowTitle(m_chat->m_tarName);
            // 显示与目标用户的历史聊天记录
            m_chat->showChatHistory(m_chat->m_tarName);
            // 显示聊天框
            m_chat->show();
        }
    }
    else
    {
        // 不聊天，直接返回
        qDebug()<<"不聊天";
        return;
    }
}
// 选择好友，点击聊天按钮进行聊天
void Friend::on_chat_PB_clicked()
{
    // 获取选中的内容
    QListWidgetItem* pItem =  ui->friend_LW->currentItem();
    // 没有选中，报错
    if(!pItem)
    {
        QMessageBox::information(this,"好友聊天","请选择你要聊天的好友");
        return;
    }
    // 获取 当前用户名 和 目标用户名
    m_chat->m_curName = Client::getInstance().getLoginName();;
    m_chat->m_tarName =  pItem->text();;
    // 如果该界面是隐藏的，则进行展示
    if(m_chat->isHidden())
    {
        qDebug()<<"打开聊天界面";
        // 设置聊天框的标题为 目标用户用户名
        m_chat->setWindowTitle(m_chat->m_tarName);
        // 显示与目标用户的历史聊天记录
        m_chat->showChatHistory(m_chat->m_tarName);
        // 显示聊天框
        m_chat->show();
    }
}
