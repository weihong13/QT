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

void File::flushFileReq()
{

    PDU* pdu = initPDU(m_curPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_curPath.toStdString().c_str(),m_curPath.toStdString().size());
    qDebug()<<"File flushFile pdu->caMsg"<< pdu->caMsg;
    Client::getInstance().sendPDU(pdu);


}

// 更新文件列表框的函数
void File::updateFileList(QList<FileInfo*> fileList)
{
    // 每次更新文件列表框前，需要将上一次在的文件列表信息释放，避免内存泄漏
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        delete pFileInfo;
    }

    // 清空 存储文件信息列表 成员变量
    m_fileInfoList.clear();
    // 给成员变量 赋上新的值
    m_fileInfoList = fileList;
    // 清空之前文件列表框的内容
    ui->file_LW->clear();
    // 将fileList中每个文件的信息挨个取出
    foreach(FileInfo* pFileInfo,fileList)
    {
        QListWidgetItem* pItem = new QListWidgetItem;
        // 设置文件名
        pItem->setText(pFileInfo->caFileName);
        qDebug()<<"File updateFileList pFileInfo caFileName"<< pFileInfo->caFileName;
        // 设置文件图标
        if(pFileInfo->uiFileType == ENUM_FILE_TYPE_FOLDER)
        {
            pItem->setIcon(QIcon(QPixmap(":/fileIcon/folder.png")));
        }
        else if(pFileInfo->uiFileType == ENUM_FILE_TYPE_TXT)
        {
            pItem->setIcon(QIcon(QPixmap(":/:/fileIcon/txt.png")));
        }
        ui->file_LW->addItem(pItem);
    }
    // 展示新的文件列表框的内容
    ui->file_LW->show();

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



void File::on_flushFile_PB_clicked()
{
    flushFileReq();
}
