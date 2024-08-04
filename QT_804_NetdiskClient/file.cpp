#include "client.h"
#include "file.h"
#include "ui_file.h"
#include "index.h"

#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_rootPath = QString("%1/%2")
            .arg(Client::getInstance().getRootPath())
            .arg(Client::getInstance().getLoginName());
    m_curPath = m_rootPath;
    qDebug()<<"File m_rootPath"<<m_rootPath;
    qDebug()<<"File m_curPath"<<m_curPath;

}

File::~File()
{
    delete ui;
}

QString File::getRootPath()
{
    return m_rootPath;
}

QString File::getCurPath()
{
    return m_curPath;
}

// 创建文件夹
void File::on_mkdir_PB_clicked()
{

    // 输入框 获取要创建的文件夹名称
    QString strFolderName = QInputDialog::getText(this,"创建文件夹","文件夹名称：");
    if(strFolderName.isEmpty())
    {
        QMessageBox::critical(this,"输入错误","用户名不能为空");
        return;
    }
    if(strFolderName.toStdString().size() > 32)
    {
        QMessageBox::critical(this,"输入错误","文件夹名称长度应小于32位");
        return;
    }
    // 获取当前路径
    QString curFileName = m_curPath;


    // 构建pdu
    PDU* pdu = initPDU(curFileName.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_REQUEST;
    memcpy(pdu->caData,strFolderName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,curFileName.toStdString().c_str(),curFileName.toStdString().size());
    qDebug()<<"mkdir_PB_clicked pdu->caData"<<pdu->caData;
    qDebug()<<"mkdir_PB_clicked pdu->caMsg"<<pdu->caMsg;

    Client::getInstance().sendPDU(pdu);


}
