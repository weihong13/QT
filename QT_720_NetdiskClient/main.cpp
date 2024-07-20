#include "client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Client w; // 构造函数私有化了，无法使用
//    w.show();

    // 使用静态成员函数 获取单例对象
    Client::getInstance().show();

    return a.exec();
}
