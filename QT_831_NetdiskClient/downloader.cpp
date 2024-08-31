#include "downloader.h"
#include "client.h"

#include <QThread>


Downloader::Downloader()
{

}

Downloader::Downloader(const QString strDownloadFilePath, qint64 downloadFileSize):
    m_strDownloadFilePath(strDownloadFilePath)
{
    m_downloadFileSize = downloadFileSize;
}

void Downloader::start()
{
    // 创建一个线程对象
    QThread *thread = new QThread;
    // 将当前这个类放入线程中执行
    this->moveToThread(thread);

    // 线程开始信号，绑定上传文件函数
    connect(thread, &QThread::started, this, &Downloader::downloadFile);
    // 上传文件完成信号，触发线程退出
    connect(this, &Downloader::finished, thread, &QThread::quit);
    // 线程结束后，触发线程释放
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(&Client::getInstance(), &Client::downloadData, this, &Downloader::downloadFileData);
    // 启动线程
    thread->start();
}

void Downloader::downloadFile()
{
    // 是否打开文件成功
    bool ret = false;
    // 设置下载的路径
    m_downloadFile.setFileName(m_strDownloadFilePath);
    // 打开文件
    if(m_downloadFile.open(QIODevice::WriteOnly))
    {
        // 打开文件成功
        ret = true;
        emit bDownload();
        m_downloadRecvSize = 0;
    }
    else
    {
        emit errorOccurred("下载文件时，打开失败");
    }

    PDU* pdu = initPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_REQUEST;
    memcpy(pdu->caData,&ret,sizeof(bool));
    emit downloadPDU(pdu);
}

void Downloader::downloadFileData(char *buf, int size)
{
    // 将数据写入到文件中
    m_downloadFile.write(buf,size);
    // 计算已下载的数据大小
    m_downloadRecvSize += size;
    // 判断是否下载完成
    if(m_downloadRecvSize < m_downloadFileSize)
    {
        return;
    }
    // 关闭文件
    m_downloadFile.close();

    // 判断是否下载成功
    if(m_downloadRecvSize == m_downloadFileSize)
    {
        emit finished();
        return;
    }
    else
    {
        emit errorOccurred("下载文件失败，下载出错");
        emit finished();
        return;
    }

}
