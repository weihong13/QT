#ifndef SELECTPATH_H
#define SELECTPATH_H

#include "protocol.h"

#include <QWidget>
#include <qlistwidget.h>

namespace Ui {
class SelectPath;
}

class SelectPath : public QWidget
{
    Q_OBJECT

public:
    explicit SelectPath(QWidget *parent = nullptr);
    ~SelectPath();

    // 刷新文件
    void flushFileReq();

    // 更新文件列表框的函数
    void updateFileList(QList<FileInfo*> fileList);


    QList<FileInfo*> m_fileInfoList;

    // 用户的根目录
    QString m_rootPath;
    // 当前目录
    QString m_curPath;

    // 用户要保存的文件名
    QString m_fileName;

    QString m_sharePath;



private slots:
    void on_mkdir_PB_clicked();

    void on_return_PB_clicked();

    void on_confirm_PB_clicked();

    void on_cancel_PB_clicked();

    void on_selectPath_LW_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::SelectPath *ui;
};

#endif // SELECTPATH_H
