#ifndef FILE_H
#define FILE_H

#include "movefile.h"

#include <QListWidgetItem>
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

    // 刷新文件
    void flushFileReq();

    // 获取文件上传状态
    bool& getUpload();

    // 更新文件列表框的函数
    void updateFileList(QList<FileInfo*> fileList);

    // 上传文件中的数据
    void uploadFileData();


    QList<FileInfo*> m_fileInfoList;
    MoveFile* m_moveFile;



private slots:
    void on_mkdir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_rmdir_PB_clicked();

    void on_rmFile_PB_clicked();

    void on_file_LW_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_renameFile_PB_clicked();

    void on_moveFile_PB_clicked();

    void on_uploadFile_PB_clicked();

private:
    Ui::File *ui;
    // 用户的根目录
    QString m_rootPath;
    // 用户的当前目录
    QString m_curPath;
    // 是否正在上传
    bool m_bUpload;
    // 当前上传文件的路径
    QString m_strUploadFilePath;


};

#endif // FILE_H
