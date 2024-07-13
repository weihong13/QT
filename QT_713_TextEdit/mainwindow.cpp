#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QFontComboBox>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>

#include <QColor>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QIcon>
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(800,600);

    // 菜单栏
    this->set_MenuBar();

    // 工具栏
    this->set_ToolBar();

    this->set_Central();

    this->set_Status();




}
 // 设置菜单栏
void MainWindow::set_MenuBar()
{
    this->menu = new QMenuBar(this);
    this->setMenuBar(menu);

    // 设置文件菜单
    this->set_FileMenu();

    // 设置编辑菜单
    set_EditMenu();

}

// 设置文件菜单
void MainWindow::set_FileMenu()
{
    FileMenu =  menu->addMenu("文件");
    // QAction* newFile = new QAction;
    //newFile = FileMenu->addAction(QIcon(":/img/CreateNewFile.png"),"新建");
    FileMenu->addAction(QIcon(":/img/OpenFile.png"),"打开");
    FileMenu->addAction(QIcon(":/img/save.png"),"保存");
    FileMenu->addAction(QIcon(":/img/save-as.png"),"另存为");
    FileMenu->addAction(QIcon(":/img/exit.png"),"退出");
    menu->addMenu(FileMenu);
}
// 设置编辑菜单
void MainWindow::set_EditMenu()
{
    EditMenu =  menu->addMenu("编辑");
    EditMenu->addAction(QIcon(":/img/revoke.png"),"撤销");
    EditMenu->addAction(QIcon(":/img/Counter-Revocation.png"),"反撤销");
    EditMenu->addAction(QIcon(":/img/SelectAll.png"),"全选");
    EditMenu->addAction(QIcon(":/img/copy.png"),"复制");
    EditMenu->addAction(QIcon(":/img/paste.png"),"粘贴");
    EditMenu->addAction(QIcon(":/img/cut.png"),"剪切");
    menu->addMenu(EditMenu);
}


// 设置工具栏
void MainWindow::set_ToolBar()
{
    this->ToolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea,ToolBar);
    ToolBar->setMovable(false);
    // 设置字体
    set_Font();
    // 设置字号
    set_FontSize();
    // 设置加粗
    set_FontBold();
    // 设置倾斜
    set_FontSlanted();
    // 设置颜色
    set_FontColor();

}

// 设置字体
void MainWindow::set_Font()
{
    QLabel* FontLabel = new QLabel("字体：",ToolBar);
    ToolBar->addWidget(FontLabel);
    QFontComboBox* FontComoBox = new QFontComboBox(FontLabel);
    ToolBar->addWidget(FontComoBox);
    QObject::connect(FontComoBox,&QFontComboBox::currentFontChanged,[this](const QFont &font){
        this->TextEdit->setFont(font);
    });

}
// 设置字号
void MainWindow::set_FontSize()
{
    QLabel* FontSizeLabel = new QLabel(" 字号：",ToolBar);
    ToolBar->addWidget(FontSizeLabel);
    QComboBox* ComboBox = new QComboBox(FontSizeLabel);
    ToolBar->addWidget(ComboBox);
    for(int i = 8;i<=22;i+=2)
    {
        ComboBox->addItem(QString::number(i));
    }
    QObject::connect(ComboBox,&QComboBox::currentTextChanged,[this](const QString &size){

       this->TextEdit->setFontPointSize(size.toDouble());
    });

}
// 设置加粗
void MainWindow::set_FontBold()
{
    QPushButton* BoldBnt = new QPushButton(QIcon(":/img/FontBold.png"),"加粗",ToolBar);
    ToolBar->addWidget(BoldBnt);
    QObject::connect(BoldBnt,&QPushButton::clicked,[this](){

       this->TextEdit->setFontWeight(TextEdit->fontWeight()==QFont::Bold?QFont::Normal:QFont::Bold);
    });
}

// 设置倾斜
void MainWindow::set_FontSlanted()
{
    QPushButton* SlantedBnt = new QPushButton(QIcon(":/img/SlantedFonts.png"),"倾斜",ToolBar);
    ToolBar->addWidget(SlantedBnt);
    QObject::connect(SlantedBnt,&QPushButton::clicked,[this](){
       this->TextEdit->setFontItalic(TextEdit->fontItalic()==1?0:1);
    });
}

// 设置颜色
void MainWindow::set_FontColor()
{
    QPushButton* ColorBnt = new QPushButton(QIcon(":/img/FonColor.png"),"颜色",ToolBar);
    ToolBar->addWidget(ColorBnt);
    QObject::connect(ColorBnt,&QPushButton::clicked,[this](){
        QColor color = QColorDialog::getColor();
        this->TextEdit->setTextColor(color);
    });

}


// 设置中心控件
void MainWindow::set_Central()
{
    TextEdit = new QTextEdit(this);
    this->setCentralWidget(TextEdit);
}

// 设置状态栏
void MainWindow::set_Status()
{
    this->StatusBar = new QStatusBar(this);
    this->setStatusBar(StatusBar);
    QLabel* TimeLabel = new QLabel("时间：",StatusBar);
    StatusBar->addWidget(TimeLabel);
    QLabel* Time = new QLabel(StatusBar);
    StatusBar->addWidget(Time);

    QTimer* timer = new QTimer(StatusBar);
    timer->start(1000);
    QObject::connect(timer,&QTimer::timeout,[Time](){
       QString str = QDateTime::currentDateTime().toString(" yyyy年-MM月-dd日 hh:mm:ss ");//设置显示的格式
       Time->setText(str);
    });
}



MainWindow::~MainWindow()
{
    delete ui;
}

