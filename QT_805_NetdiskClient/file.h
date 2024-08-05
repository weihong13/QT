#ifndef FILE_H
#define FILE_H

#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    // 获取用户的根目录
    QString getRootPath();
    // 获取用户的当前目录
    QString getCurPath();

private slots:
    void on_mkdir_PB_clicked();

private:
    Ui::File *ui;
    // 用户的根目录
    QString m_rootPath;
    // 用户的当前目录
    QString m_curPath;
};

#endif // FILE_H
