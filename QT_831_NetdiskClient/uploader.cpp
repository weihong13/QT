#include "uploader.h"

#include <QThread>

Uploader::Uploader()
{

}

// 初始化参数列表
Uploader::Uploader(const QString &filePath):m_strUploadFilePath(filePath),m_fUploadFile(filePath)
{

}

// 启动线程
void Uploader::start()
{
    // 创建一个线程对象
    QThread *thread = new QThread;
    // 将当前这个类放入线程中执行
    this->moveToThread(thread);

    // 线程开始信号，绑定上传文件函数
    connect(thread, &QThread::started, this, &Uploader::uploadFile);
    // 上传文件完成信号，触发线程退出
    connect(this, &Uploader::finished, thread, &QThread::quit);
    // 线程结束后，触发线程释放
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    // 启动线程
    thread->start();

}

// 上传文件函数
void Uploader::uploadFile()
{

     QFile file(m_strUploadFilePath);
     if(!file.open(QIODevice::ReadOnly))
     {
         // 无法使用qdebug打印日志，只能使用信号发送给主函数去打印
         emit errorOccurred("打开文件失败");
         emit finished();
         return;
     }
     qint64 readCount;
     while (true)
     {
         PDU* dataPdu = initPDU(4096);
         dataPdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
         readCount = file.read(dataPdu->caMsg, 4096);
         if (readCount == 0)
         {
             break;
         }
         if (readCount < 0)
         {
             emit errorOccurred("上传文件失败：读出文件失败");
             break;
         }
         // 修改实际消息大小 已经 pdu的长度（节省空间）
         dataPdu->uiMsgLen = readCount;
         dataPdu->uiPDULen = readCount+sizeof(PDU);
         // 触发发送pdu的信号
         // 这里不能直接使用socket，直接将pdu发给主线程，然后主线程去利用socket发送数据
         // 如果多线程同时使用socket的话，就会出错
         emit uploadPDU(dataPdu);
     }

     // 关闭文件
     file.close();
     // 发送上传完成信号
     emit finished();

}











