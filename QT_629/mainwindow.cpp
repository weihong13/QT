#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->resize(800,600);

    // 创建菜单栏,菜单栏可以包含多个菜单，每个菜单都可以包含多个活动项
    QMenuBar* menu = new QMenuBar(this);
    this->setMenuBar(menu);
    // 添加菜单
    QMenu* FileMenu = menu->addMenu("文件");
    QMenu* EditMenu = menu->addMenu("编辑");
    QMenu* BuildMenu = menu->addMenu("构建");

    // 给菜单添加活动项
    FileMenu->addAction("新建文件或项目");
    FileMenu->addAction("打开文件或项目");
    FileMenu->addAction("关闭文件");

    EditMenu->addAction("全选");
    EditMenu->addAction("复制");
    EditMenu->addAction("粘贴");


}

MainWindow::~MainWindow()
{
    delete ui;
}

