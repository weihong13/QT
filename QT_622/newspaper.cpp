#include "newspaper.h"

// 构造函数
Newspaper::Newspaper(QObject *parent) : QObject(parent)
{

}

Newspaper::Newspaper(QString str)
{
    this->m_Str = str;
}
// 发送信号的函数实现
void Newspaper::sendSignals()
{
    // emit 宏定义，用来调用信号函数发出信号。
    emit Newspaper::newpaper();
}
// 将成员变量 m_Str 的值传给 信号函数，实现有参传递
void Newspaper::setStr()
{
    emit Newspaper::newpaper(m_Str);
}

