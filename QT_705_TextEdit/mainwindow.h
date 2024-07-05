#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSize>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



    // 设置菜单栏
    void setMenu();

    // 设置工具栏
    void setToolBar();

    // 设置中心控件
    void setText();
    // 设置状态栏
    void setStatus();

    // 打开文件
    void openFile();


private slots:
    void TimeUpdate();

private:
    Ui::MainWindow *ui;
    QMenuBar *menu;
    QToolBar *tool;
    QLabel *currentTimeLabel; // 先创建一个QLabel对象
    QStatusBar* stb;

};
#endif // MAINWINDOW_H
