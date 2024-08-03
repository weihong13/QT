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

    // 消息聊天框获取消息
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    // 获取时间
    void dealMessageTime(QString curMsgTime);
    // 响应消息
    void friendChatRes(const char* curName,const char* msg);

    QString m_curName;
    QString m_tarName;
    Ui::Chat *ui;
    ~Chat();
protected:
    // 调整事件
    void resizeEvent(QResizeEvent *event);
private slots:
    // 发送消息的槽函数
    void on_pushButton_clicked();

private:

};

#endif // CHAT_H
