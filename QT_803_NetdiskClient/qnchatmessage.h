#ifndef QNCHATMESSAGE_H
#define QNCHATMESSAGE_H

#include <QWidget>

class QPaintEvent;
class QPainter;
class QLabel;
class QMovie;

class QNChatMessage : public QWidget
{
    Q_OBJECT
public:
    explicit QNChatMessage(QWidget *parent = nullptr);
    // 用户类型
    enum User_Type{
        User_System,//系统
        User_Me,    //自己
        User_She,   //用户
        User_Time,  //时间
    };
    // 设置消息发送成功
    // void setTextSuccess();

    // 设置消息
    void setText(QString text, QString time, QSize allSize, User_Type userType);

    QSize getRealString(QString src);
    QSize fontRect(QString str);

    inline QString text() {return m_msg;}
    inline QString time() {return m_time;}
    inline User_Type userType() {return m_userType;}
protected:
    void paintEvent(QPaintEvent *event);
private:

    // 消息的属性
    QString m_msg; // 实际消息
    QString m_time;// 发送消息的时间
    QString m_curTime;// 当前时间
    QSize m_allSize;// 所有控件的大小信息
    User_Type m_userType = User_System;// 消息类型，默认是系统消息

    // 气泡的属性
    int m_kuangWidth; // 消息框的宽度
    int m_textWidth;  // 文本的宽度
    int m_spaceWid;   //
    int m_lineHeight; // 行高

    // 位置属性
    QRect m_iconLeftRect;
    QRect m_iconRightRect;
    QRect m_sanjiaoLeftRect;
    QRect m_sanjiaoRightRect;
    QRect m_kuangLeftRect;
    QRect m_kuangRightRect;
    QRect m_textLeftRect;
    QRect m_textRightRect;
    QPixmap m_leftPixmap;
    QPixmap m_rightPixmap;

};

#endif // QNCHATMESSAGE_H
