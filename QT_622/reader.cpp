#include "reader.h"

// 构造函数
Reader::Reader(QObject *parent) : QObject(parent)
{

}

// 用来打印
void Reader::reader()
{
    qDebug()<<"收到！";
}

// 重载
void Reader::reader(QString str)
{
    qDebug() << str;
}
