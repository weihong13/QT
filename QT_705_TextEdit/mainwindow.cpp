#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFontComboBox>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>
#include <QTextEdit>
#include <QStatusBar>

#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QIcon>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(800,600);

    // 菜单栏
    this->setMenu();

    // 工具栏
    this->setToolBar();

    this->setText();

    this->setStatus();




}
 // 设置菜单栏
void MainWindow::setMenu()
{
    this->menu = new QMenuBar(this);
    this->setMenuBar(menu);

    QMenu* FileMenu =  menu->addMenu("文件");
    QAction* newFile = new QAction;
    newFile = FileMenu->addAction(QIcon(":/img/CreateNewFile.png"),"新建");



    connect(newFile,SIGNAL(triggered()),menu,SLOT(open()));


    FileMenu->addAction(QIcon(":/img/OpenFile.png"),"打开");
    FileMenu->addAction(QIcon(":/img/save.png"),"保存");
    FileMenu->addAction(QIcon(":/img/save-as.png"),"另存为");
    FileMenu->addAction(QIcon(":/img/exit.png"),"退出");
    menu->addMenu(FileMenu);


    QMenu* EditMenu =  menu->addMenu("编辑");
    EditMenu->addAction(QIcon(":/img/revoke.png"),"撤销");
    EditMenu->addAction(QIcon(":/img/Counter-Revocation.png"),"反撤销");
    EditMenu->addAction(QIcon(":/img/SelectAll.png"),"全选");
    EditMenu->addAction(QIcon(":/img/copy.png"),"复制");
    EditMenu->addAction(QIcon(":/img/paste.png"),"粘贴");
    EditMenu->addAction(QIcon(":/img/cut.png"),"剪切");
    menu->addMenu(EditMenu);
}
// 设置工具栏
void MainWindow::setToolBar()
{
    this->tool = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea,tool);
    tool->setMovable(false);
    tool->addAction("字体：");

    QFontComboBox* fcb = new QFontComboBox(tool);
    tool->addWidget(fcb);

    QComboBox* cb = new QComboBox(tool);

    tool->addWidget(cb);

    QPushButton* bnt1 = new QPushButton(tool);
    bnt1->setText("加粗");
    tool->addWidget(bnt1);

    QPushButton* bnt2 = new QPushButton(tool);
    bnt2->setText("倾斜");
    tool->addWidget(bnt2);

    QPushButton* bnt3 = new QPushButton(tool);
    bnt3->setText("颜色");
    tool->addWidget(bnt3);

    QDialog* dial = new QColorDialog(bnt3);

}
// 设置中心控件
void MainWindow::setText()
{
    QTextEdit* textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);

}

// 设置状态栏
void MainWindow::setStatus()
{
    this->stb = new QStatusBar(this);
    this->setStatusBar(stb);

    stb->addWidget(new QLabel("时间："));

    this->currentTimeLabel = new QLabel(stb); // 创建QLabel控件
    QDateTime CurrentTime=QDateTime::currentDateTime();
    QString Timestr=CurrentTime.toString(" yyyy年-MM月-dd日 hh:mm:ss "); //设置显示的格式
    currentTimeLabel->setText(Timestr);
    stb->addWidget(currentTimeLabel); //在状态栏添加此控件
    QTimer *timer = new QTimer(stb);
    timer->start(1000); //每隔1000ms发送timeout的信号
    QObject::connect(timer, SIGNAL(timeout()),stb,SLOT(TimeUpdate()));


}

void MainWindow::TimeUpdate()
{
    QDateTime CurrentTime=QDateTime::currentDateTime();
    QString Timestr=CurrentTime.toString(" yyyy年-MM月-dd日 hh:mm:ss "); //设置显示的格式
    currentTimeLabel->setText(Timestr);
    stb->addWidget(currentTimeLabel);
}

// 打开文件
void MainWindow::openFile()
{
    QFileDialog::getOpenFileName();

}

MainWindow::~MainWindow()
{
    delete ui;
}

