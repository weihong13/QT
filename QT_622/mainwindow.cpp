#include "mainwindow.h"
#include "ui_mainwindow.h"


// 某个自定义的类想使用信号槽，这个类MianWindow就必须继承 QObject（或者是他的祖先类有继承QObject）
// 而且在类内第一行必须有 Q_OBJECT // 信号槽，宏定义
// QObject 基本上是所有类的基类。
// 这里 MainWindow类继承了 QMainWindow类，而QMainWindow类继承了QWidget类
// 而QWidget类继承了QObject类
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) // 创建ui中的 MainWindow对象
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

