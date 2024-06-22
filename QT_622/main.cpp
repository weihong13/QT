#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    QPushButton bnt(&w); // 有参构造，传入parent参数，保证新的按钮在 mianwindow窗口中，
//    bnt.show();
//    bnt.move(100,100);

//    // 某个自定义的类想使用信号槽，这个类MianWindow就必须继承 QObject（或者是他的祖先类有继承QObject）
//    // 而且在类内第一行必须有 Q_OBJECT // 信号槽，宏定义
//    // QObject 基本上是所有类的基类。
//    QPushButton bnt1(&w);
//    bnt1.show();
//    bnt1.move(200,200);
//    bnt1.setText("退出");

//    // 对象链接（绑定）
//    // 参数：发送信号的对象，发送的信号，接收信号的对象，接收信号后对象要执行的函数
//    QObject::connect(&bnt1,&QPushButton::clicked,&w,&MainWindow::close);

//    return a.exec(); // 循环队列，接收事件
//}



// 报社发送报纸，读者接收
#include "newspaper.h"
#include "reader.h"
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    // 定义两个对象
//    Newspaper n;
//    Reader r;


//    // 链接对象，newpaper信号，链接reader函数。
//    QObject::connect(&n,&Newspaper::newpaper,&r,&Reader::reader);
//    n.sendSignals(); // 手动发送信号

//    // 创建一个按钮
//    QPushButton bnt1(&w);
//    bnt1.show();

//    // 让按钮的点击信号，链接newpaper信号，而newpaper信号又链接了 reader函数
//    // 因此，点击按钮即可打印收到
//    QObject::connect(&bnt1,&QPushButton::clicked,&n,&Newspaper::newpaper);

//    return a.exec(); // 循环队列，接收事件
//}

// 对信号函数和信号槽函数的重载
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // 定义两个对象
    Newspaper new_paper("hello");
    Reader reader;

    void (Newspaper::*nfunc1)() = &Newspaper::newpaper;
    void (Newspaper::*nfunc2)(QString) = &Newspaper::newpaper;
    void (Newspaper::*nfunc3)() = &Newspaper::setStr;

    void (Reader::*rfunc1)() = &Reader::reader;
    void (Reader::*rfunc2)(QString) = &Reader::reader;

    // 调用无参
    // 链接对象，newpaper信号，链接reader函数。
    QObject::connect(&new_paper,nfunc1,&reader,rfunc1);
    new_paper.sendSignals(); // 手动发送信号
    // 创建一个按钮
    QPushButton bnt1(&w);
    bnt1.show();
    bnt1.move(100,100);
    bnt1.setText("无参");
    // 让按钮的点击信号，链接newpaper信号，而newpaper信号又链接了 reader函数
    // 因此，点击按钮即可打印收到
    QObject::connect(&bnt1,&QPushButton::clicked,&new_paper,nfunc1);




    // 有参调用
    QObject::connect(&new_paper,nfunc2,&reader,rfunc2);
    // 创建一个按钮
    QPushButton bnt2(&w);
    bnt2.show();
    bnt2.move(100,200);
    bnt2.setText("有参");

    QObject::connect(&bnt2,&QPushButton::clicked,&new_paper,nfunc3);

    return a.exec(); // 循环队列，接收事件
}
