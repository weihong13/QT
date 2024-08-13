#ifndef REQHANDLER_H
#define REQHANDLER_H


#include "protocol.h"

#include <QFile>
#include <QObject>
class MyTcpSocket;

class ReqHandler : public QObject
{
    Q_OBJECT
public:
    explicit ReqHandler(QObject *parent = nullptr);
    // 处理注册请求
    PDU* regist();
    // 处理登录请求
    PDU* login(QString& loginName);
    // 处理查找用户请求
    PDU* findUser();
    // 处理在线用户请求
    PDU* onlineUser();

    // 处理添加好友请求
    PDU* addFriendReq();

    // 处理同意添加好友请求
    void addFriendRes();

    // 刷新好友列表请求
    PDU* flushFriend(QString& curName);
    // 删除好友请求
    PDU* deleteFriend(QString& curName);

    // 好友聊天--用于向目标客户端转发聊天内容
    void friendChat();

    // 创建文件夹
    PDU* mkdir();

    // 刷新文件
    PDU* flushFile();

    // 删除文件夹
    PDU* rmdir();

    // 删除文件
    PDU* rmFile();

    // 重命名文件
    PDU* renameFile();

    // 刷新文件
    PDU* moveFlushFile();

    // 移动文件
    PDU* moveFile();

    // 上传文件
    PDU* uploadFile();

    // 上传文件中的数据
    PDU* uploadFileData();

    // 下载文件请求
    PDU* downloadFile();

    // 下载文件数据请求
    PDU* downloadFileData(MyTcpSocket* mySocket);

    // 分享文件请求
    PDU* sharFile();

    // 刷新文件
    PDU* selectFlushFile();

    // 同意分享文件
    PDU* sharFileAgree();

    // 复制文件夹
    bool copyDir(QString strSrcDir,QString strDestDir);

    PDU* m_pdu;

signals:

private:
    // 是否正在上传
    bool m_bUpload;
    // 上传路径
    QFile m_uploadPath;
    // 文件总大小
    qint64 m_uploadFileSize;
    // 已经接收的大小
    qint64 m_uploadRecvSize;

    //
    bool m_bDownload;
    // 要下载文件的路径
    QFile m_downloadPath;

};

#endif // REQHANDLER_H
