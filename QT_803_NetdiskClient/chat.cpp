#include "chat.h"
#include "protocol.h"
#include "ui_chat.h"
#include "client.h"

#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    // 主窗口关闭，在线用户窗口也会跟着关闭
    setAttribute(Qt::WA_QuitOnClose,false);
    resize(600, 800);
}



Chat::~Chat()
{
    delete ui;
}



void Chat::on_pushButton_clicked()
{
    // 获取输入框中的内容
    QString msg = ui->textEdit->toPlainText();
    ui->textEdit->setText("");

    if(msg.isEmpty())
    {
        QMessageBox::information(this,"发送消息","发送的消息不能为空");
        return;
    }

    //时间戳
    QString time = QString::number(QDateTime::currentDateTime().toTime_t());

    // 打印 消息，时间戳 和 消息框中的条目数
    qDebug()<<"addMessage" << msg << time << ui->listWidget->count();

    // 消息框中显示自己的消息
    dealMessageTime(time);
    QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);


    // 构建PDU发送消息到服务器
    int uiMsgLen =  msg.toStdString().size();
    PDU* pdu = initPDU(uiMsgLen);

    pdu->uiMsgType = ENUM_MSG_TYPE_FRIEND_CHAT_REQUEST;
    memcpy(pdu->caData,m_curName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,m_tarName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,msg.toStdString().c_str(),uiMsgLen);
    // 测试
    qDebug() << "on_pushButton_clicked caData" << pdu->caData;
    qDebug() << "on_pushButton_clicked caData+32" << pdu->caData+32;
    qDebug() << "on_pushButton_clicked caMsg" << pdu->caMsg;

    Client::getInstance().sendPDU(pdu);

    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);

}


void Chat::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->listWidget->setItemWidget(item, messageW);
}

void Chat::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);

        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}

// 响应消息
void Chat::friendChatRes(const char* curName,const char* msg)
{
    // 判空
    if(curName == NULL || msg ==NULL)
    {
        return;
    }
    // 如果该界面是隐藏的，则进行展示
    if(this->isHidden())
    {
       qDebug()<<"打开聊天界面";
       this->setWindowTitle(curName);
       this->show();
    }
    // 给当前客户端与目标客户端赋值
    m_curName = Client::getInstance().getLoginName();
    m_tarName = curName;
    //时间戳
    QString time = QString::number(QDateTime::currentDateTime().toTime_t());

    // 打印 消息，时间戳 和 消息框中的条目数
    qDebug()<<"addMessage" << msg << time << ui->listWidget->count();

    // 消息框中显示自己的消息
    dealMessageTime(time);
    QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    dealMessage(messageW, item, msg, time, QNChatMessage::User_She);
}

// 在聊天框中加载所有信息
void Chat::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);


    ui->textEdit->resize(this->width() - 20, ui->widget->height() - 20);
    ui->textEdit->move(10, 10);

    ui->pushButton->move(ui->textEdit->width()+ui->textEdit->x() - ui->pushButton->width() - 10,
                         ui->textEdit->height()+ui->textEdit->y() - ui->pushButton->height() - 10);


    for(int i = 0; i < ui->listWidget->count(); i++) {
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        QListWidgetItem* item = ui->listWidget->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}
