#ifndef UPLOADER_H
#define UPLOADER_H

#include "protocol.h"

#include <QFile>
#include <QObject>

class Uploader : public QObject
{
    Q_OBJECT
public:
    Uploader();
    Uploader(const QString &filePath);
    // 启动线程
    void start();

public slots:
    // 上传文件槽函数
    void uploadFile();

signals:
    // 上传文件信号
    void uploadPDU(PDU* pdu);
    // 完成信号
    void finished();
    // 错误信号
    void errorOccurred(const QString & error);

private:
    QString m_strUploadFilePath;
    QFile m_fUploadFile;
};

#endif // UPLOADER_H
