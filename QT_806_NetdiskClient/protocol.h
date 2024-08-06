#ifndef PROTOCOL_H
#define PROTOCOL_H

// 重命名 unsigned int
typedef unsigned int uint;

// 消息类型的枚举值
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0, // 消息类型的最小值

    // 每一种消息类型一般都有两种，一个用来进行请求，一个接收响应。
    ENUM_MSG_TYPE_REGIST_REQUEST, // 注册用户的请求
    ENUM_MSG_TYPE_REGIST_RESPOND, // 注册用户的响应

    ENUM_MSG_TYPE_LOGIN_REQUEST, // 登录的请求
    ENUM_MSG_TYPE_LOGIN_RESPOND, // 登录的响应

    ENUM_MSG_TYPE_FIND_USER_REQUEST, // 查找用户的请求
    ENUM_MSG_TYPE_FIND_USER_RESPOND, // 查找用户的响应

    ENUM_MSG_TYPE_ONLINE_USER_REQUEST, // 在线用户的请求
    ENUM_MSG_TYPE_ONLINE_USER_RESPOND, // 在线用户的响应

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, // 添加好友的请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, // 添加好友的响应


    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST, // 刷新好友的请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, // 刷新好友的响应

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, // 删除好友的请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, // 删除好友的响应

    ENUM_MSG_TYPE_FRIEND_CHAT_REQUEST, // 好友聊天的请求
    ENUM_MSG_TYPE_FRIEND_CHAT_RESPOND, // 好友聊天的响应

    ENUM_MSG_TYPE_MKDIR_REQUEST, // 创建文件夹的请求
    ENUM_MSG_TYPE_MKDIR_RESPOND, // 创建文件夹的响应

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST, // 刷新文件夹的请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND, // 刷新文件夹的响应


    ENUM_MSG_TYPE_MAX=0x00ffffff, // 消息类型的最大值

};

// 协议数据单元- Protocol Data Unit
struct PDU
{
     uint uiPDULen;  // 协议的总长度
     uint uiMsgType; // 消息的类型
     uint uiMsgLen;  // 实际的消息长度
     char caData[64];// 参数
     char caMsg[];   // 柔性数组，用来存放实际的消息
};


// 文件类型枚举值
enum ENUM_FILE_TYPE
{
    ENUM_FILE_TYPE_MIN=0, // 消息类型的最小值

    ENUM_FILE_TYPE_FOLDER,// 文件夹
    ENUM_FILE_TYPE_TXT,   // 文本文件


    ENUM_FILE_TYPE_MAX=0x00ffffff, // 消息类型的最大值
};


// 文件信息结构体
struct FileInfo
{
    char caFileName[32];
    uint uiFileType;

};

// 定义PDU初始化函数
PDU* initPDU(uint uiMsgLen);

#endif // PROTOCOL_H
