#ifndef READER_H
#define READER_H

#include <QObject>
#include <QDebug>

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QObject *parent = nullptr);
    // 打印函数的声明，信号槽函数，
    void reader();
    // 信号槽函数的重载声明
    void reader(QString str);


signals:

public slots:
    // 信号槽函数在这里声明，但是一个类的所有函数都能做信号槽函数，因此在public中声明也可以。
};

#endif // READER_H
