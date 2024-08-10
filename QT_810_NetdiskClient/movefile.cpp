#include "movefile.h"
#include "ui_movefile.h"
#include "index.h"
#include "client.h"

#include <QInputDialog>
#include <QMessageBox>

MoveFile::MoveFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoveFile)
{
    ui->setupUi(this);

    // 获取用户根目录
    m_rootPath = QString("%1/%2")
            .arg(Client::getInstance().getRootPath())
            .arg(Client::getInstance().getLoginName());
    m_curPath = m_rootPath;

    // 主窗口关闭，移动文件窗口也会跟着关闭
    setAttribute(Qt::WA_QuitOnClose,false);
}

MoveFile::~MoveFile()
{
    delete ui;
}

void MoveFile::flushFileReq()
{
    PDU* pdu = initPDU(m_curPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_curPath.toStdString().c_str(),m_curPath.toStdString().size());
    qDebug()<<"File flushFile pdu->caMsg"<< pdu->caMsg;
    Client::getInstance().sendPDU(pdu);
}

void MoveFile::updateFileList(QList<FileInfo *> fileList)
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
    ui->moveFile_LW->clear();
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
        ui->moveFile_LW->addItem(pItem);
    }
    // 展示新的文件列表框的内容
    ui->moveFile_LW->show();
}






void MoveFile::on_mkdir_PB_clicked()
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

void MoveFile::on_return_PB_clicked()
{
    // 判断当前是否为根目录（用户的目录，而不是服务器根目录）
    // 删除最后一级的目录（根据 / 删除），得到上一级的路径
    // 发送刷新目录请求

    // 判断当前目录是否为用户的根目录
    if(m_curPath == m_rootPath)
    {
        QMessageBox::warning(this,"返回上一级","返回失败：当前目录已是根目录");
        return;
    }

    int index = m_curPath.lastIndexOf('/');
    m_curPath.remove(index,m_curPath.size()-index);

    // 发送刷新请求
    flushFileReq();
}

void MoveFile::on_moveFile_LW_itemDoubleClicked(QListWidgetItem *item)
{
    // 取到选择的文件名，判断是否为文件夹，如果是文件夹，则刷新列表
    QString strFileName = item->text();
    // 判断是否为文件夹
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        if(pFileInfo->caFileName == strFileName && pFileInfo->uiFileType != ENUM_FILE_TYPE_FOLDER)
        {

            return;
        }
    }
    // 更新当前路径
    m_curPath = QString("%1/%2").arg(m_curPath).arg(strFileName);

    // 发送刷新请求
    flushFileReq();
}

void MoveFile::on_confirm_PB_clicked()
{

    // 获取当前选中的列表框
    QListWidgetItem* pItem = ui->moveFile_LW->currentItem();
    // 没选择内容
    if(pItem == NULL)
    {
        QMessageBox::information(this,"移动文件","请选择要移动到的目录");
        return;
    }
    // 获取选中的文件夹名称
    QString strCurFolder = pItem->text();
    // 判断是否为文件夹
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        if(pFileInfo->caFileName == strCurFolder && pFileInfo->uiFileType != ENUM_FILE_TYPE_FOLDER)
        {
            QMessageBox::information(this,"移动文件","请选择文件夹");
            return;
        }
    }
    //
    int ret = QMessageBox::No;
    // 文件要移动的路径
    QString strMovePath = QString("%1/%2").arg(m_curPath).arg(strCurFolder);
    if(m_beforePath ==  strMovePath)
    {
        QMessageBox::critical(this,"移动文件","不能将文件移动到自身目录");
        return;
    }
    else
    {
        // 询问是否移动
        ret = QMessageBox::question(this,"移动文件",QString("是否将文件移动到该目录：%1").arg(strMovePath));
    }

    // 不移动
    if(ret == QMessageBox::No)
    {
        return;
    }
    // 文件的完整的原始路径
    QString strSrcFilePath = QString("%1/%2").arg(m_beforePath).arg(m_moveFileName);
    // 文件的完整的移动后路径
    QString strTarFilePath = QString("%1/%2").arg(strMovePath).arg(m_moveFileName);

    qDebug()<<"MoveFile confirm_PB strSrcFilePath"<<strSrcFilePath;
    qDebug()<<"MoveFile confirm_PB strTarFilePath"<<strTarFilePath;

    int srcLen = strSrcFilePath.toStdString().size();
    int tarLen = strTarFilePath.toStdString().size();

    PDU* pdu = initPDU(srcLen+tarLen+1);

    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;

    memcpy(pdu->caData,&srcLen,sizeof(int));
    memcpy(pdu->caData+32,&tarLen,sizeof(int));
    memcpy(pdu->caMsg,strSrcFilePath.toStdString().c_str(),srcLen);
    memcpy(pdu->caMsg+srcLen,strTarFilePath.toStdString().c_str(),tarLen);

    Client::getInstance().sendPDU(pdu);

}



void MoveFile::on_cancel_PB_clicked()
{
    this->close();
}
