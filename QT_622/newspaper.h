#ifndef NEWSPAPER_H
#define NEWSPAPER_H

#include <QObject>

class Newspaper : public QObject
{
    Q_OBJECT
public:
    explicit Newspaper(QObject *parent = nullptr);
    Newspaper(QString);
    // 发送信号的函数声明
    void sendSignals();

    void setStr();

    QString m_Str;

signals:
    // 信号函数，必须在 signals下声明，返回值只能是void，不用实现函数体
    // 信号函数可以有参数，因此可以重载
    void newpaper();
    // 信号函数的重载
    void newpaper(QString str);

};

#endif // NEWSPAPER_H
