#include "reshandler.h"
#include "client.h"
#include "index.h"

#include <QMessageBox>

ResHandler::ResHandler(QObject *parent) : QObject(parent)
{

}

// 注册用户响应
void ResHandler::regist()
{
    // 将消息取出
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    qDebug()<<"regist ret: "<<ret;
    // 根据返回的响应进行处理
    if(ret)
    {
        QMessageBox::information(&Client::getInstance(),"注册","注册成功");
    }
    else
    {
        QMessageBox::information(&Client::getInstance(),"注册","注册失败：用户名或密码非法");
    }
}

// 登录响应
void ResHandler::login()
{
    // 将消息取出
    bool ret;
    char caName[32] = {'\0'};
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    memcpy(caName,m_pdu->caData+32,32);
    // 根据返回的响应进行处理
    if(ret)
    {
        Client::getInstance().getLoginName() = caName;
        qDebug()<<"recvMsg LOGIN_REQUEST m_userName"<<Client::getInstance().getLoginName();
        QMessageBox::information(&Client::getInstance(),"登录","登录成功");

        // 隐藏登录界面
        Client::getInstance().hide();
        qDebug()<<"recvMsg Client hide";
        // 登录成功后，跳转到首页
        Index::getInstance().show();
        qDebug()<<"recvMsg Index show";
        // 登录成功后，刷新好友列表
        Index::getInstance().getFriend()->flushFriendReq();
        qDebug()<<"recvMsg login flushFriendReq";


    }
    else
    {
        QMessageBox::information(&Client::getInstance(),"登录","登录失败：用户名或密码非法");
    }
}

// 查找用户响应
void ResHandler::findUser()
{
    // 将消息取出
    char caName[32] = {'\0'};
    int ret;
    memcpy(&ret,m_pdu->caData,sizeof(int));
    memcpy(caName,m_pdu->caData+32,32);
    // 根据返回的响应进行处理
    if(ret == -1)
    {
        QMessageBox::information(&Index::getInstance(),"查找用户",QString("用户 %1 不存在").arg(caName));
        return;
    }
    else if(ret == 0)
    {
        QMessageBox::information(&Index::getInstance(),"查找用户",QString("用户 %1 不在线").arg(caName));
        return;
    }
    else if(ret == 1)
    {
        int res = QMessageBox::question(&Index::getInstance(),"查找用户",QString("用户 %1 在线，是否添加好友？").arg(caName),"添加好友","取消");
        if(res == 0)
        {
            // 获取当前用户名 和 目标用户名
            QString strCurName =  Client::getInstance().getLoginName();
            QString strTarName = caName;
            // 测试
            qDebug()<<"findUser addFriend strCurName:"<<strCurName;
            qDebug()<<"findUser addFriend strTarName:"<<strTarName;
            // 初始化pdu
            PDU* pdu = initPDU(0);
            // 添加消息类型为 添加好友请求
            pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            // 添加数据
            memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
            memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
            // 测试--打印检测发送的内容
            qDebug()<<"findUser addFriend uiMsgType: "<<pdu->uiMsgType;
            qDebug()<<"findUser addFriend strCurName: "<<pdu->caData;
            qDebug()<<"findUser addFriend strTarName: "<<pdu->caData+32;

            // 发送消息
            Client::getInstance().sendPDU(pdu);
        }
        else
        {
            return;
        }

    }
    else
    {
        QMessageBox::critical(&Index::getInstance(),"查找用户",QString("查找用户 %1 失败，内部错误").arg(caName));
        return;
    }

}

// 在线用户响应
void ResHandler::onlineUser(QString &loginName)
{
    // 获取在线用户的个数
    uint listSize = m_pdu->uiMsgLen/32;
    qDebug()<<"listSize  "<<listSize;
    // 创建变量存储在线用户的用户名
    char userName[32];
    QStringList nameList;
    // 将caMsg中的用户名挨个取出，并放到nameList中
    for(uint i = 0; i <listSize; i++)
    {
        // 挨个取出用户名
        memcpy(userName,m_pdu->caMsg+i*32,32);
        // 测试
        // qDebug()<<"ONLINE_USER_RESPOND  "<<QString(userName);
        // 跳过自己
        if(QString(userName) == loginName)
        {
            continue;
        }
        // 将取到的用户名存放到 nameList中
        nameList.append(QString(userName));
    }
    // 调用展示在线用户的函数
    Index::getInstance().getFriend()->m_onlineUser->showOnlineUser(nameList);
}


// 处理其他客户发来的添加好友请求
void ResHandler::addFriendReq()
{
    // 取出Cur客户端的用户名
    char curName[32] = {'\0'};
    memcpy(curName,m_pdu->caData,32);
    int ret =  QMessageBox::question(Index::getInstance().getFriend(),"添加好友",QString("是否同意 %1 的添加好友请求？").arg(curName),"同意添加","拒绝添加");
    if(ret == 0)
    {
        qDebug()<<"ResHandler addFriendRequest QMessageBox::Yes";
        PDU* pdu = initPDU(sizeof(int));
        ret = 1;
        qDebug()<<"ResHandler addFriendRequest ret : "<<ret;
        pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        memcpy(pdu->caData,m_pdu->caData,64);
        memcpy(pdu->caMsg,&ret,sizeof(int));

        // 测试--打印检测发送的内容
        qDebug()<<"ResHandler addFriendRequest uiMsgType: "<<pdu->uiMsgType;
        qDebug()<<"ResHandler addFriendRequest curName: "<<pdu->caData;
        qDebug()<<"ResHandler addFriendRequest tarName: "<<pdu->caData+32;
        qDebug()<<"ResHandler addFriendRequest ret: "<<pdu->caMsg;
        // 发送消息
        Client::getInstance().sendPDU(pdu);
    }
    else
    {
        qDebug()<<"ResHandler addFriendRequest QMessageBox::no";
        PDU* pdu = initPDU(sizeof(int));
        ret = 2;
        pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        memcpy(pdu->caData,m_pdu->caData,64);
        memcpy(pdu->caMsg,&ret,sizeof(int));
        // 发送消息

        // 测试--打印检测发送的内容
        qDebug()<<"ResHandler addFriendRequest uiMsgType: "<<pdu->uiMsgType;
        qDebug()<<"ResHandler addFriendRequest curName: "<<pdu->caData;
        qDebug()<<"ResHandler addFriendRequest tarName: "<<pdu->caData+32;
        qDebug()<<"ResHandler addFriendRequest ret : "<<pdu->caMsg;

        Client::getInstance().sendPDU(pdu);
    }
}

// 处理添加好友的响应
void ResHandler::addFriendRes()
{
    // 将消息取出
    int ret;
    char tarName[32] = {'\0'};
    memcpy(tarName,m_pdu->caData+32,32);
    memcpy(&ret,m_pdu->caMsg,sizeof(int));

    qDebug()<<"ResHandler addFriendRes tarName："<<tarName;
    qDebug()<<"ResHandler addFriendRes ret："<<ret;

    // 根据返回的响应进行处理
    if(ret == -2)
    {
        QMessageBox::information(&Index::getInstance(),"添加好友","该用户已经是你的好友！");
        return;
    }
    else if(ret == 0)
    {
        QMessageBox::information(&Index::getInstance(),"添加好友","该用户已下线");
        return;
    }
    else if(ret == 1)
    {
        QMessageBox::information(&Index::getInstance(),"添加好友",QString("已经成功添加用户 %1 为好友！").arg(tarName));
        Index::getInstance().getFriend()->flushFriendReq();
        return;
    }
    else if(ret == 2)
    {
        QMessageBox::information(&Index::getInstance(),"添加好友",QString("用户 %1 不同意添加请求，添加好友失败！").arg(tarName));
        return;
    }

    else
    {
        QMessageBox::critical(&Index::getInstance(),"添加好友","添加好友错误！");
        return;
    }

}


// 刷新好友响应
void ResHandler::flushFriend()
{
    // 获取在线用户的个数
    uint listSize = m_pdu->uiMsgLen/32;
    qDebug()<<"listSize  "<<listSize;
    // 创建变量存储在线用户的用户名
    char friendName[32];
    QStringList friendList;
    // 将caMsg中的用户名挨个取出，并放到nameList中
    for(uint i = 0; i <listSize; i++)
    {
        // 挨个取出用户名
        memcpy(friendName,m_pdu->caMsg+i*32,32);
        // 测试
        qDebug()<<"ResHandler flushFriend  friendName"<<QString(friendName);


        // 将取到的用户名存放到 nameList中
        friendList.append(QString(friendName));
    }
    // 调用展示在线用户的函数
    Index::getInstance().getFriend()->showFirend(friendList);
}

// 删除好友响应
void ResHandler::deleteFriend()
{
    // 取出响应结果
    int ret;
    memcpy(&ret,m_pdu->caData,sizeof(int));
    if(ret ==1)
    {
        QMessageBox::information(&Index::getInstance(),"删除好友","删除好友成功！");
        // 删除好友成功后，刷新好友列表
        Index::getInstance().getFriend()->flushFriendReq();

        return;
    }
    else if(ret == -1)
    {
        QMessageBox::information(&Index::getInstance(),"删除好友","该好友已经不是你的好友了！");
        // 删除好友成功后，刷新好友列表
        Index::getInstance().getFriend()->flushFriendReq();

    }
    else
    {

        QMessageBox::information(&Index::getInstance(),"删除好友","删除好友出错！");
        return;
    }
}

// 其他好友聊天的请求
void ResHandler::friendChat()
{
    // 取出发来消息的用户，和发来的消息
    char curName[32] = {'\0'};
    char msg[m_pdu->uiMsgLen+1];
    memcpy(curName,m_pdu->caData,32);
    memcpy(msg,m_pdu->caMsg,m_pdu->uiMsgLen);

    // 测试--打印检测发送的内容
    qDebug()<<"ResHandler friendChat uiMsgType: "<<m_pdu->uiMsgType;
    qDebug()<<"ResHandler friendChat curName: "<<m_pdu->caData;
    qDebug()<<"ResHandler friendChat tarName: "<<m_pdu->caData+32;
    qDebug()<<"ResHandler friendChat ret : "<<m_pdu->caMsg;
    msg[m_pdu->uiMsgLen] = '\0';
    QString strMsg = QString(msg);
    // 调用 响应好友聊天的函数
    Index::getInstance().getFriend()->m_chat->friendChatRes(curName, strMsg);


}

// 创建文件夹响应
void ResHandler::mkdir()
{
    // 取出响应结果
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(ret)
    {
        QMessageBox::information(&Index::getInstance(),"创建文件夹","创建成功！");
        // 创建文件夹成功后，调用刷新文件请求
        Index::getInstance().getFile()->flushFileReq();
        if(!Index::getInstance().getFile()->m_moveFile->isHidden())
        {
            Index::getInstance().getFile()->m_moveFile->flushFileReq();
        }
        return;
    }
    else
    {

        QMessageBox::information(&Index::getInstance(),"创建文件夹","创建失败！");
        return;
    }
}

// 刷新文件响应
void ResHandler::flushFile()
{
    // 计算文件信息列表的大小
    int FileListCount = m_pdu->uiMsgLen/sizeof (FileInfo);

    QList<FileInfo*> fileList;

    // 挨个将每个文件的信息，放到 fileList列表中
    for(int i = 0; i<FileListCount; i++)
    {
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo,m_pdu->caMsg+i*sizeof (FileInfo),sizeof (FileInfo));
        // 测试 打印每个文件的文件名
        qDebug()<<"ResHandler flushFile pFileInfo caFileName"<<pFileInfo->caFileName;
        // 将每个文件的文件信息结构体添加到 列表中
        fileList.append(pFileInfo);
    }
    // 调用更新文件列表框的函数
    Index::getInstance().getFile()->updateFileList(fileList);

}

// 删除文件响应
void ResHandler::rmdir()
{

    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof (bool));

    if(ret)
    {
        QMessageBox::information(Index::getInstance().getFile(),"删除文件夹","删除文件夹成功");

    }
    else
    {
        QMessageBox::information(Index::getInstance().getFile(),"删除文件夹","删除文件夹失败");
    }
    Index::getInstance().getFile()->flushFileReq();
}

// 删除文件响应
void ResHandler::rmFile()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof (bool));

    if(ret)
    {
        QMessageBox::information(Index::getInstance().getFile(),"删除文件","删除文件成功");

    }
    else
    {
        QMessageBox::information(Index::getInstance().getFile(),"删除文件","删除文件失败");
    }
    Index::getInstance().getFile()->flushFileReq();
}

// 重命名文件
void ResHandler::renameFile()
{
    // 取出重命名文件响应的返回值
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof (bool));

    if(!ret)
    {
        QMessageBox::information(Index::getInstance().getFile(),"重命名文件","重命名文件失败");
    }
    Index::getInstance().getFile()->flushFileReq();
}

// 移动文件时的刷新文件响应
void ResHandler::moveFlushFile()
{
    // 计算文件信息列表的大小
    int FileListCount = m_pdu->uiMsgLen/sizeof (FileInfo);

    QList<FileInfo*> fileList;

    // 挨个将每个文件的信息，放到 fileList列表中
    for(int i = 0; i<FileListCount; i++)
    {
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo,m_pdu->caMsg+i*sizeof (FileInfo),sizeof (FileInfo));
        // 测试 打印每个文件的文件名
        qDebug()<<"ResHandler flushFile pFileInfo caFileName"<<pFileInfo->caFileName;
        // 将每个文件的文件信息结构体添加到 列表中
        fileList.append(pFileInfo);
    }
    // 调用移动文件的更新文件列表框的函数
    Index::getInstance().getFile()->m_moveFile->updateFileList(fileList);
}

// 移动文件响应
void ResHandler::moveFile()
{
    // 取出重命名文件响应的返回值
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof (bool));
    if(ret)
    {
        QMessageBox::information(Index::getInstance().getFile(),"移动文件","移动文件成功");
    }
    else
    {
        QMessageBox::information(Index::getInstance().getFile(),"移动文件","移动文件失败");
    }
    Index::getInstance().getFile()->m_moveFile->close();
    Index::getInstance().getFile()->flushFileReq();
}

// 上传文件响应
void ResHandler::uploadFile()
{
    // 上传文件响应的返回值
    int ret;
    memcpy(&ret,m_pdu->caData,sizeof (int));
    if(ret == 0)
    {
        qDebug()<<"1111";
        Index::getInstance().getFile()->uploadFileData();
        qDebug()<<"2222";
    }
    else if(ret == 1)
    {
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","当前已有文件在上传");
    }
    else
    {
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","打开文件失败");
    }

}

// 上传文件数据的响应
void ResHandler::uploadFileData()
{
    // 取出重命名文件响应的返回值
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof (bool));
    if(ret)
    {    qDebug()<<"ResHandler uploadFileData ret 1";
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","上传文件成功");
        Index::getInstance().getFile()->flushFileReq();
    }
    else
    {
        qDebug()<<"ResHandler uploadFileData ret 0";
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","上传文件失败");
    }
    Index::getInstance().getFile()->getUpload() = false;
}






