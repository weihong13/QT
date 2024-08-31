#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "protocol.h"

#include <QObject>
#include <qfile.h>

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader();
    Downloader(const QString strDownloadFilePath,qint64 downloadFileSize);
    // 启动线程
    void start();


    // 要下载到的本机地址
    QString m_strDownloadFilePath;
    // 打开的文件
    QFile m_downloadFile;
    // 下载文件的总大小
    qint64 m_downloadFileSize;
    // 已经接收的大小
    qint64 m_downloadRecvSize;

public slots:
    void downloadFile();
    // 下载文件数据槽函数
    void downloadFileData(char *buf, int size);

signals:
    // 上传文件信号
    void downloadPDU(PDU* pdu);

    // 打开文件信号
    void bDownload();
    // 完成信号
    void finished();
    // 错误信号
    void errorOccurred(const QString & error);

private:

};

#endif // DOWNLOADER_H
