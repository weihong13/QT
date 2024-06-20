#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // qt的命令，用于处理参数

    MainWindow w;
    w.show();
    QPushButton bnt(&w);
    bnt.show();
    bnt.move(100,100);
    bnt.setText("hello QT!"); // 先构造一个 QString 类型的对象，然后再进行传参

    QPushButton bnt1(&w);

    bnt1.show();
    bnt1.move(200,200);
    bnt1.setText("退出");

    // 实现点击退出，信号槽
    // 使用信号槽之前，得进行对象绑定（链接），才能接收到信号

    // 链接函数
    // connect 接收的四个对象
    /*
    connect()函数最常用的一般形式:
    connect(sender, signal,receiver,slot);
    参数:
    sender:发出信号的对象
    signal:发送对象发出的信号
    receiver:接收信号的对象
    slot:接收对象在接收到信号之后所需要调用的函
    */
    QObject::connect(&bnt1,&QPushButton::clicked,&w,&MainWindow::close);




    return a.exec(); // exex函数族，循环队列，循环处理事件
}
