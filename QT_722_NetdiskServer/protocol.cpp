#include "protocol.h"

#include <stdlib.h>
#include <string.h>

// PDU初始化函数
PDU* initPDU(uint uiMsgLen)
{
    // 计算协议总长度
    uint uiPDULen = sizeof (PDU)+uiMsgLen;
    // 申请堆区内存
    PDU* pdu = (PDU*)malloc(uiPDULen);
    // 未申请到空间，退出
    if(pdu == NULL)
    {
        exit(1);
    }
    // 初始化堆区内容，防止有残留数据
    memset(pdu,0,uiPDULen);
    // 给成员变量赋值
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;
    // 返回指针
    return pdu;
}
