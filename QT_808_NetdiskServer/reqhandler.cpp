#include "operatedb.h"
#include "reqhandler.h"
#include "mytcpserver.h"
#include "server.h"


#include <QDebug>
#include <QDir>

ReqHandler::ReqHandler(QObject *parent) : QObject(parent)
{

}

PDU *ReqHandler::regist()
{
    // 将消息取出
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,m_pdu->caData,32);
    memcpy(caPwd,m_pdu->caData+32,32);
    // 测试
    qDebug()<<"ReqHandler regist caName: "<<caName;
    qDebug()<<"ReqHandler regist caPwd: "<<caPwd;

    // 处理消息
    bool ret = OperateDB::getInstance().handleRegist(caName,caPwd);

    if(ret)
    {
        // 如果注册成功，给该用户添加一个单独的目录
        QDir dir;
        dir.mkdir(QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(caName));
    }

    // 向客户端发送响应
    // 初始化响应注册的PDU对象
    PDU* registPdu = initPDU(0);
    // 消息类型为注册响应
    registPdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    // 将消息存储到消息结构体
    memcpy(registPdu->caData,&ret,sizeof(bool));

    qDebug()<<"ReqHandler registPdu ret: "<<registPdu->caData;

    return registPdu;
}

PDU *ReqHandler::login(QString& loginName)
{
    // 将消息取出
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,m_pdu->caData,32);
    memcpy(caPwd,m_pdu->caData+32,32);
    // 测试
    qDebug()<<"ReqHandler login caName: "<<caName;
    qDebug()<<"ReqHandler login caPwd: "<<caPwd;

    // 处理消息
    bool ret = OperateDB::getInstance().handleLogin(caName,caPwd);
    if(ret)
    {
        // 登录成功，记录登陆成功的用户名
        loginName = caName;
    }
    // 向客户端发送响应
    // 初始化响应登录的PDU对象
    PDU* loginPdu = initPDU(0);
    // 消息类型为注册响应
    loginPdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    // 将消息存储到消息结构体
    memcpy(loginPdu->caData,&ret,sizeof(bool));
    memcpy(loginPdu->caData+32,caName,32);

    return loginPdu;
}

// 处理查找用户的请求
PDU *ReqHandler::findUser()
{
    // 将消息取出
    char caName[32] = {'\0'};
    memcpy(caName,m_pdu->caData,32);
    // 测试
    qDebug()<<"ReqHandler findUser caName: "<<caName;

    // 处理消息
    int ret = OperateDB::getInstance().handleFindUser(caName);
    // 向客户端发送响应
    // 初始化响应查找用户的PDU对象
    PDU* findUserPdu = initPDU(0);
    // 消息类型为查找用户响应
    findUserPdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_RESPOND;
    // 将消息存储到消息结构体

    memcpy(findUserPdu->caData,&ret,sizeof(int));
    memcpy(findUserPdu->caData+32,caName,32);

    return findUserPdu;
}

PDU *ReqHandler::onlineUser()
{
    // 处理消息
    QStringList nameList = OperateDB::getInstance().handleOnlineUser();

    // 获取列表大小
    int listSize = nameList.size();
    uint uiMsgLen = listSize*32;

    // 向客户端发送响应
    // 初始化响应在线用户的PDU对象
    PDU* onlineUserPdu = initPDU(uiMsgLen);
    // 消息类型为在线用户响应
    onlineUserPdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USER_RESPOND;


    // 将用户名 挨个放到 caMsg中
    for(int i = 0; i<listSize; i++)
    {
        // 测试
        // qDebug()<<"ReqHandler  onlineUser " << nameList.at(i);

        // 将每一个用户名都存储到 caMsg中
        memcpy(onlineUserPdu->caMsg+i*32,nameList.at(i).toStdString().c_str(),32);
    }

    return onlineUserPdu;
}

// 处理添加好友的请求
PDU *ReqHandler::addFriendReq()
{
    // 取出用户名
    char curName[32] = {'\0'};
    char tarName[32] = {'\0'};
    memcpy(curName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    // 测试
    qDebug()<<"ReqHandler addFriendReq curName: "<<curName;
    qDebug()<<"ReqHandler addFriendReq tarName: "<<tarName;

    // 在数据库中判断当前客户端与目标客户端是否已经是好友，以及目标客户端是否在线
    int ret = OperateDB::getInstance().handleAddFriend(curName,tarName);
    qDebug()<<"ReqHandler addFriendReq ret: "<<ret;
    // 根据数据库响应，决定是否要转发
    if(ret==1)
    {
        MyTcpServer::getInstance().resend(tarName,m_pdu);
        return NULL;

    }
    else
    {
        // 如果添加过程出现错误，直接返回 添加好友的响应
        // 初始化响应查找用户的PDU对象
        PDU* resPdu = initPDU(sizeof (int));
        // 消息类型为添加好友的响应
        resPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        // 将消息存储到消息结构体
        memcpy(resPdu->caData,curName,32);
        memcpy(resPdu->caData+32,tarName,32);
        memcpy(resPdu->caMsg,&ret,sizeof(int));
        return resPdu;
    }

}

// 处理 是否同意添加好友的请求
void ReqHandler::addFriendRes()
{
    // 取出用户名，以及目标用户的态度
    char curName[32] = {'\0'};
    char tarName[32] = {'\0'};
    int ret;
    memcpy(curName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    memcpy(&ret,m_pdu->caMsg,sizeof(int));

    // 测试--打印检测接收的内容
    qDebug()<<"ResHandler addFriendRes uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler addFriendRes curName: "<<curName;
    qDebug()<<"ResHandler addFriendRes tarName: "<<tarName;
    qDebug()<<"ResHandler addFriendRes ret : "<<ret;
    if(ret==1)
    {
        // 目标用户同意添加好友
        // 建立好友关系
        int res = OperateDB::getInstance().handleAddFriendAgree(curName,tarName);

        // 构建发回当前用户的pdu
        PDU* curPdu = initPDU(sizeof(int));
        curPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;

        memcpy(curPdu->caData,curName,32);
        memcpy(curPdu->caData+32,tarName,32);
        // 将消息存储到消息结构体
        memcpy(curPdu->caMsg,&res,sizeof(int));
        // 转发添加响应
        MyTcpServer::getInstance().resend(curName,curPdu);

        // 构建发回目标用户的pdu
        PDU* tarPdu = initPDU(sizeof(int));
        tarPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        // 将消息存储到消息结构体
        memcpy(tarPdu->caData,tarName,32);
        memcpy(tarPdu->caData+32,curName,32);
        memcpy(tarPdu->caMsg,&res,sizeof(int));

        // 将添加结果转发回目标用户
        MyTcpServer::getInstance().resend(tarName,tarPdu);


        // 释放
        free(curPdu);
        curPdu = NULL;
        free(tarPdu);
        tarPdu = NULL;
    }
    else
    {
        // 目标用户拒绝添加好友
        qDebug()<<"ResHandler addFriendRes ret 2 : ";
        m_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        MyTcpServer::getInstance().resend(curName,m_pdu);
    }
}
// 刷新好友列表请求
PDU *ReqHandler::flushFriend(QString& curName)
{
    // 处理消息
    QStringList friendList = OperateDB::getInstance().handleFlushFriend(curName);

    // 获取好友列表大小
    int listSize = friendList.size();
    uint uiMsgLen = listSize*32;

    // 向客户端发送响应
    // 初始化响应刷新好友的PDU对象
    PDU* flushFriendPdu = initPDU(uiMsgLen);
    // 消息类型为刷新好友的响应
    flushFriendPdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;


    // 将用户名 挨个放到 caMsg中
    for(int i = 0; i<listSize; i++)
    {
        // 测试
        qDebug()<<"ReqHandler  flushFriend " << friendList.at(i);

        // 将每一个用户名都存储到 caMsg中
        memcpy(flushFriendPdu->caMsg+i*32,friendList.at(i).toStdString().c_str(),32);
    }

    return flushFriendPdu;
}

PDU *ReqHandler::deleteFriend(QString &curName)
{
    // 取出要删除的用户名
    char tarName[32] = {'\0'};
    memcpy(tarName,m_pdu->caData,32);

    // 测试--打印检测发送的内容
    qDebug()<<"ResHandler deleteFriend uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler deleteFriend curName: "<<m_pdu->caData;
    // 处理消息，获取返回值
    int ret = OperateDB::getInstance().handleDeleteFriend(curName,tarName);
    // 构建响应PDU
    PDU* resPdu = initPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    memcpy(resPdu->caData,&ret,sizeof(int));

    return resPdu;
}

// 好友聊天--用于向目标客户端转发聊天内容
void ReqHandler::friendChat()
{
    // 取出目标用户名
    char tarName[32] = {'\0'};

    memcpy(tarName,m_pdu->caData+32,32);

    // 测试--打印检测发送的内容
    qDebug()<<"ResHandler friendChat uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler friendChat curName: "<<m_pdu->caData;
    qDebug()<<"ResHandler friendChat tarName: "<<m_pdu->caData+32;
    qDebug()<<"ResHandler friendChat caMsg : "<<m_pdu->caMsg;

    // 将消息转发给目标用户
    MyTcpServer::getInstance().resend(tarName,m_pdu);


}


// 创建文件夹
PDU *ReqHandler::mkdir()
{
    // 取出文件名和当前路径
    char strFileName[32] = {'\0'};
    char strCurFile[m_pdu->uiMsgLen];

    memcpy(strFileName,m_pdu->caData,32);
    memcpy(strCurFile,m_pdu->caMsg,m_pdu->uiMsgLen);


    qDebug()<<"ReqHandler mkdir strFolderName"<<strFileName;
    qDebug()<<"ReqHandler mkdir strCurFile"<<strCurFile;

    // 构建响应的resPdu
    PDU* resPdu = initPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_RESPOND;
    bool ret = false;
    QDir dir;
    // 如果当前目录不存在，报错
    if(!dir.exists(strCurFile))
    {
        memcpy(resPdu->caData,&ret,sizeof(bool));
        return resPdu;
    }
    QString strNewFile = QString("%1/%2").arg(strCurFile).arg(strFileName);
    // 如果新的目录存在，或者新建文件夹出错，报错
    if(dir.exists(strNewFile) || !dir.mkdir(strNewFile))
    {
        memcpy(resPdu->caData,&ret,sizeof(bool));
        return resPdu;
    }
    // 创建新的文件夹成功
    ret = true;
    memcpy(resPdu->caData,&ret,sizeof(bool));
    return resPdu;


}

// 刷新文件
PDU *ReqHandler::flushFile()
{
    // 获取当前路径
    char curFile[m_pdu->uiMsgLen];
    memcpy(curFile,m_pdu->caMsg,m_pdu->uiMsgLen);
    QString strCurFile = QString(curFile);
    qDebug()<<"ReqHandler flushFile strCurFile"<<strCurFile;

    // 判空
    if(strCurFile.isEmpty())
    {
        return NULL;
    }
    // 获取当前文件夹下的所以文件信息
    QDir dir(strCurFile);
    QFileInfoList fileInfoList =  dir.entryInfoList();
    // 获取文件信息列表的大小
    int fileInfoCount = fileInfoList.size();
    qDebug()<<"ReqHandler flushFile fileInfoCount"<<fileInfoCount;

    // 按照文件信息列表的大小，构建pdu
    PDU* resPdu = initPDU(sizeof (FileInfo)*(fileInfoCount-2));
    resPdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
    // 创建一个文件信息结构体
    FileInfo* pFileInfo = NULL;

    QString fileName;
    // 挨个获取当前文件夹下的每个文件的信息
    for(int i = 0,j = 0; i < fileInfoCount; i++)
    {
        // 获取文件名
        fileName = fileInfoList[i].fileName();
        // 去除 . 和 .. 文件
        if(fileName == QString(".") || fileName == QString("..")) continue;
        qDebug()<<"ReqHandler flushFile fileName"<<fileName;
        // 将char* 类型的caMsg 转换为 FileInfo*
        pFileInfo = (FileInfo*)resPdu->caMsg + (j++);
        // 将文件名 存放到 caMsg中
        memcpy(pFileInfo->caFileName,fileName.toStdString().c_str(),32);
        // 存放 文件类型
        if(fileInfoList[i].isDir())
        {
            pFileInfo->uiFileType = ENUM_FILE_TYPE_FOLDER;
        }
        else if(fileInfoList[i].isFile())
        {
            pFileInfo->uiFileType = ENUM_FILE_TYPE_TXT;
        }

    }

    return resPdu;
}

// 删除文件夹
PDU *ReqHandler::rmdir()
{
    // 获取文件路径
    char* filePath = m_pdu->caMsg;
    qDebug()<<"ReqHandler rmdir filePath: "<<filePath;
    bool ret = false;

    QFileInfo fileInfo(filePath);
    // 判断是否为文件夹
    if(fileInfo.isDir())
    {

        QDir dir(filePath);
        // 删除目录下的所以文件
        ret = dir.removeRecursively();
    }

    qDebug()<<"ReqHandler rmdir ret: "<<ret;
    PDU* resPdu = initPDU(0);

    resPdu->uiMsgType = ENUM_MSG_TYPE_RMDIR_RESPOND;

    memcpy(resPdu->caData, &ret, sizeof (bool));

    return resPdu;

}

// 删除文件
PDU *ReqHandler::rmFile()
{
    // 获取文件所在目录
    char* curPath = m_pdu->caMsg;
    // 获取文件名
    char fileName[32] = {'\0'};
    memcpy(fileName,m_pdu->caData,32);
    // 将文件名转为字符串
    QString strFileName = QString(fileName);
    // 将当前路径与选中 的文件名拼接，得到完整的文件路径
    QString strFilePath = QString("%1/%2").arg(curPath).arg(strFileName);

    qDebug()<<"ReqHandler rmFile strFilePath: "<<strFilePath;
    qDebug()<<"ReqHandler rmFile strFileName: "<<strFileName;
    // 是否成功删除、默认失败
    bool ret = false;

    QFileInfo fileInfo(strFilePath);
    // 判断是否为文件
    if(!fileInfo.isDir())
    {
        // 设置文件所在的目录
        QDir dir(curPath);
        // 删除文件
        ret = dir.remove(strFileName);
        qDebug()<<"ReqHandler rmFile ret: "<<ret;
    }
    // 构建响应pdu
    PDU* resPdu = initPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_RMFILE_RESPOND;
    memcpy(resPdu->caData, &ret, sizeof (bool));
    return resPdu;
}




