#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMenuBar>
#include <QToolBar>
#include <QTextEdit>
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
    void set_MenuBar();
    // 设置文件菜单
    void set_FileMenu();
    // 设置编辑菜单
    void set_EditMenu();


    // 设置工具栏
    void set_ToolBar();
    // 设置字体
    void set_Font();
    // 设置字号
    void set_FontSize();
    // 设置加粗
    void set_FontBold();
    // 设置倾斜
    void set_FontSlanted();
    // 设置颜色
    void set_FontColor();



    // 设置中心控件
    void set_Central();

    // 设置状态栏
    void set_Status();


private slots:



private:
    Ui::MainWindow *ui;
    QMenuBar *menu;
    QMenu* FileMenu;
    QMenu* EditMenu;

    QToolBar *ToolBar;

    QTextEdit* TextEdit;
    QStatusBar* StatusBar;


};
#endif // MAINWINDOW_H
