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

// 定义PDU初始化函数
PDU* initPDU(uint uiMsgLen);
#endif // PROTOCOL_H
