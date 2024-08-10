#ifndef MOVEFILE_H
#define MOVEFILE_H

#include "protocol.h"

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class MoveFile;
}

class MoveFile : public QWidget
{
    Q_OBJECT

public:
    explicit MoveFile(QWidget *parent = nullptr);
    ~MoveFile();


    // 刷新文件
    void flushFileReq();

    // 更新文件列表框的函数
    void updateFileList(QList<FileInfo*> fileList);


    QList<FileInfo*> m_fileInfoList;

    // 用户的根目录
    QString m_rootPath;
    // 当前目录
    QString m_curPath;

    // 用户要移动的文件名
    QString m_moveFileName;
    // 用户移动前的路径
    QString m_beforePath;




private slots:
    void on_mkdir_PB_clicked();

    void on_return_PB_clicked();

    void on_moveFile_LW_itemDoubleClicked(QListWidgetItem *item);

    void on_confirm_PB_clicked();

    void on_cancel_PB_clicked();

private:
    Ui::MoveFile *ui;
};

#endif // MOVEFILE_H
