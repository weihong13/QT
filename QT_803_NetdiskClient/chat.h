#ifndef CHAT_H
#define CHAT_H

#include "qnchatmessage.h"

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    // 响应消息
    void friendChatRes(const char* curName,QString&  msg);
    // 展示不同好友的消息记录
    void showChatHistory(const QString& friendName);

    // 当前客户端用户名
    QString m_curName;
    // 目标用户名
    QString m_tarName;

    Ui::Chat *ui;

protected:
    // 在聊天框中加载所有信息
    void resizeEvent(QResizeEvent *event);

private slots:
    // 发送消息的槽函数
    void on_sendMsg_PB_clicked();

private:
    // 消息聊天框获取消息
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    // 获取时间
    void dealMessageTime(QString curMsgTime);
    // 保存消息记录
    void saveChatHistory(QString& msg,QString& time,QNChatMessage::User_Type type);


    // 处理自己消息的对像
    QNChatMessage* m_MyMsg;
    // 自己消息的气泡
    QListWidgetItem* m_MyItem;
    // 处理他人消息的对像
    QNChatMessage* m_SheMsg;
    // 他人消息的气泡
    QListWidgetItem* m_SheItem;

    // 不同好友的聊天记录
    QMap<QString, QStringList> chatHistory; // 键是好友名称，值是消息列表

};

#endif // CHAT_H
