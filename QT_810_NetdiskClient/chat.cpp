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


// 析构函数
Chat::~Chat()
{
    if(!m_MyMsg) delete m_MyMsg;
    if(!m_SheMsg) delete m_SheMsg;
    if(!m_MyItem) delete m_MyItem;
    if(!m_SheItem) delete m_SheItem;

    delete ui;
}
// 发送消息按钮
void Chat::on_sendMsg_PB_clicked()
{
    // 获取输入框中的内容
    QString msg = ui->inputMsg_TE->toPlainText();
    // 将输入框置为空
    ui->inputMsg_TE->setText("");
    // 判断消息是否为空
    if(msg.isEmpty())
    {
        QMessageBox::information(this,"发送消息","发送的消息不能为空");
        return;
    }


    //获取当前时间
    QString time = QString::number(QDateTime::currentDateTime().toTime_t());

    // 打印 消息，时间 和 消息框中的条目数
    qDebug()<<"addMessage" << msg << time << ui->showMsg_LW->count();
    // 保存消息到聊天记录
    saveChatHistory(msg,time,QNChatMessage::User_Me);
    // 决定是否要展示时间
    dealMessageTime(time);
    // 新建一个自己消息的对象
    m_MyMsg= new QNChatMessage(ui->showMsg_LW->parentWidget());
    // 新建每条消息的对象
    m_MyItem = new QListWidgetItem(ui->showMsg_LW);
    // 传入参数，设置消息
    dealMessage(m_MyMsg, m_MyItem, msg, time, QNChatMessage::User_Me);



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
    // 发送消息
    Client::getInstance().sendPDU(pdu);

    ui->showMsg_LW->setCurrentRow(ui->showMsg_LW->count()-1);
}


// 保存历史消息记录
void Chat::saveChatHistory(QString& msg,QString& time,QNChatMessage::User_Type type)
{
    // 如果存储消息的的Map中，没有当前的目标客户端，则新建一个
    if (!chatHistory.contains(m_tarName)) {
        chatHistory[m_tarName] = QStringList();
    }
    // 将实际消息，消息时间，消息类型，转换为字符串，用|分割
    QString message = QString("%1|%2|%3").arg(msg).arg(time).arg(type);
    // 将其存储在Map中
    chatHistory[m_tarName].append(message);
}

// 展示历史消息记录
void Chat::showChatHistory(const QString &friendName)
{
    // 清空当前聊天记录
    ui->showMsg_LW->clear();

    // 检查是否有该好友的聊天记录
    if (chatHistory.contains(friendName))
    {
        // 取出Map中的历史消息记录
        const QStringList& messages = chatHistory[friendName];
        // 遍历列表中的每一条数据
        foreach (const QString& message, messages)
        {
           // 消息格式为 "内容|时间|类型"，按照 | 将每条数据进行拆分
           QStringList parts = message.split('|');
           if (parts.size() == 3) {
               QString text = parts[0];
               QString time = parts[1];
               int ret = parts[2].toInt();
               if(ret == 1)
               {    // 在聊天框中，展示自己的消息
                    m_MyMsg= new QNChatMessage(ui->showMsg_LW->parentWidget());
                    m_MyItem = new QListWidgetItem(ui->showMsg_LW);
                    dealMessage(m_MyMsg, m_MyItem, text, time, QNChatMessage::User_Me);
               }
               else
               {    // 在聊天框中，展示他人的消息
                    m_SheMsg = new QNChatMessage(ui->showMsg_LW->parentWidget());
                    m_SheItem = new QListWidgetItem(ui->showMsg_LW);
                    dealMessage(m_SheMsg, m_SheItem, text, time, QNChatMessage::User_She);
               }
           }
        }
    }
}



// 向列表框中添加一条信息
void Chat::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type)
{
    // 设置固定宽度
    messageW->setFixedWidth(this->width());
    // 计算控件的位置
    QSize size = messageW->fontRect(text);
    // 设置消息的具体位置
    item->setSizeHint(size);
    // 传入消息、发送时间、消息位置、消息类型。得到一条消息
    messageW->setText(text, time, size, type);
    // 将消息添加到列表框中
    ui->showMsg_LW->setItemWidget(item, messageW);
}

// 决定是否显示消息发送的时间，如果当前消息与上一条消息相差一分钟，则进行显示
void Chat::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->showMsg_LW->count() > 0) {
        QListWidgetItem* lastItem = ui->showMsg_LW->item(ui->showMsg_LW->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->showMsg_LW->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(ui->showMsg_LW->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->showMsg_LW);

        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        ui->showMsg_LW->setItemWidget(itemTime, messageTime);
    }
}

// 响应消息
void Chat::friendChatRes(const char* curName,QString& msg)
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
    // 如果当前聊天的用户有变化，则加载新的聊天记录
    if(curName != m_curName)
    {
        this->setWindowTitle(curName);
        showChatHistory(curName);
    }
    // 给当前客户端与目标客户端赋值
    m_curName = Client::getInstance().getLoginName();
    m_tarName = curName;

    //时间戳
    QString time = QString::number(QDateTime::currentDateTime().toTime_t());


    // 打印 消息，时间戳 和 消息框中的条目数
    qDebug()<<"friendChatRes msg" << msg << time << ui->showMsg_LW->count();

    // 保存当前聊天记录
    saveChatHistory(msg,time,QNChatMessage::User_She);

    // 消息框中显示消息
    dealMessageTime(time);
    m_SheMsg = new QNChatMessage(ui->showMsg_LW->parentWidget());
    m_SheItem = new QListWidgetItem(ui->showMsg_LW);
    dealMessage(m_SheMsg, m_SheItem, msg, time, QNChatMessage::User_She);
}

// 在聊天框中加载所有信息
void Chat::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);


    ui->inputMsg_TE->resize(this->width() - 20, ui->widget->height() - 20);
    ui->inputMsg_TE->move(10, 10);

    ui->sendMsg_PB->move(ui->inputMsg_TE->width()+ui->inputMsg_TE->x() - ui->sendMsg_PB->width() - 10,
                         ui->inputMsg_TE->height()+ui->inputMsg_TE->y() - ui->sendMsg_PB->height() - 10);


    for(int i = 0; i < ui->showMsg_LW->count(); i++) {
        QNChatMessage* messageW = (QNChatMessage*)ui->showMsg_LW->itemWidget(ui->showMsg_LW->item(i));
        QListWidgetItem* item = ui->showMsg_LW->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}

