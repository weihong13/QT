#include "client.h"
#include "file.h"
#include "ui_file.h"
#include <QDebug>


#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_rootPath = QString("%1/%2")
            .arg(Client::getInstance().getRootPath())
            .arg(Client::getInstance().getLoginName());
    m_curPath = m_rootPath;
    m_bUpload = false;

    // 打开文件界面就刷新文件目录
    flushFileReq();

    m_moveFile =  new MoveFile;
    qDebug()<<"File m_rootPath"<<m_rootPath;
    qDebug()<<"File m_curPath"<<m_curPath;

}

File::~File()
{
    if(!m_moveFile) delete m_moveFile;
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

bool &File::getUpload()
{
    return m_bUpload;
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

// 上传文件中的数据
void File::uploadFileData()
{
    // 只读模式 打开要上传的文件
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"上传文件","打开文件失败");
        m_bUpload = false;
        return;
    }
    // 将状态改为正在上传
    m_bUpload = true;
    // 构建数据pdu，默认实际消息大小为4096
    PDU* dataPdu = initPDU(4096);
    // 消息类型为 上传文件数据请求
    dataPdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
    // 循环读取文件中的数据，每次最多读4096
    while(1)
    {
        // 读出文件中的内容
        int readCount = file.read(dataPdu->caMsg,4096);
        if(readCount == 0)
        {
            // 读取完成
            break;
        }
        if(readCount < 0)
        {
            // 读取出错
            QMessageBox::warning(this,"上传文件","上传文件失败：读取错误");
            break;
        }
        // 修改实际消息大小 已经 pdu的长度（节省空间）
        dataPdu->uiMsgLen = readCount;
        dataPdu->uiPDULen = readCount+sizeof(PDU);
        qDebug()<<"File uploadFileData uiMsgLen"<<dataPdu->uiMsgLen;
        // 利用socket write发送数据。使用sendPDU的话，发送一次就被释放了，我们要一直发送
        Client::getInstance().getTcpSocket().write((char*)dataPdu,dataPdu->uiPDULen);
    }
    qDebug()<<"File uploadFileData close";
    // 所有内容都发送完成了
    m_bUpload = false;
    file.close();
    free(dataPdu);
    dataPdu = NULL;

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

// 删除文件夹
void File::on_rmdir_PB_clicked()
{
    // 获取当前选中的列表框
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    // 没选择文件夹
    if(pItem == NULL)
    {
        QMessageBox::information(this,"删除文件夹","请选择要删除的文件夹");
        return;
    }
    // 获取选中的文件名
    QString strDelFileName = pItem->text();
    // 判断是否为文件夹
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        if(pFileInfo->caFileName == strDelFileName && pFileInfo->uiFileType != ENUM_FILE_TYPE_FOLDER)
        {
            QMessageBox::information(this,"删除文件夹","请选择文件夹");
            return;
        }
    }
    // 再次询问是否删除
    int ret = QMessageBox::question(this,"删除文件夹",QString("是否要删除文件夹：%1").arg(strDelFileName));
    // 不删除
    if(ret == QMessageBox::No)
    {
        return;
    }
    // 确认删除
    // 将当前路径与选中 的文件名拼接，得到完整的文件路径
    QString strPath = QString("%1/%2").arg(m_curPath).arg(strDelFileName);
    // 构建pdu
    PDU* pdu = initPDU(strPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_RMDIR_REQUEST;
    memcpy(pdu->caMsg,strPath.toStdString().c_str(),strPath.toStdString().size());
    // 发送消息
    Client::getInstance().sendPDU(pdu);


}




// 删除文件的槽函数
void File::on_rmFile_PB_clicked()
{

    // 获取当前选中的列表框
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    // 没选择文件夹
    if(pItem == NULL)
    {
        QMessageBox::information(this,"删除文件","请选择要删除的文件");
        return;
    }
    // 获取选中的文件名
    QString strDelFileName = pItem->text();
    // 判断是否为文件夹
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        if(pFileInfo->caFileName == strDelFileName && pFileInfo->uiFileType == ENUM_FILE_TYPE_FOLDER)
        {
            QMessageBox::information(this,"删除文件","请选择文件");
            return;
        }
    }
    // 再次询问是否删除
    int ret = QMessageBox::question(this,"删除文件",QString("是否要删除文件：%1").arg(strDelFileName));
    // 不删除
    if(ret == QMessageBox::No)
    {
        return;
    }
    // 确认删除

    // 构建pdu
    PDU* pdu = initPDU(m_curPath.toStdString().size()+1);

    pdu->uiMsgType = ENUM_MSG_TYPE_RMFILE_REQUEST;
    // 将要删除的文件名与当前文件所在的路径存放到pdu
    memcpy(pdu->caData,strDelFileName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_curPath.toStdString().c_str(),m_curPath.toStdString().size());

    // 发送消息
    Client::getInstance().sendPDU(pdu);

}

// 双击进入下级目录
void File::on_file_LW_itemDoubleClicked(QListWidgetItem *item)
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

// 返回上一级目录
void File::on_return_PB_clicked()
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

// 重命名文件
void File::on_renameFile_PB_clicked()
{
    // 选中文件，点击重命名，弹出输入框，输入新的文件名

    // 获取当前选中的列表框
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    // 没选择文件夹
    if(pItem == NULL)
    {
        QMessageBox::information(this,"重命名","请选择要重命名的文件");
        return;
    }
    // 获取选中的文件名
    QString strOldFileName = pItem->text();

    // 输入框 获取 新的文件名
    QString strNewFileName = QInputDialog::getText(this,"重命名","新文件名：");
    // 合法性判断
    if(strOldFileName == strNewFileName)
    {
        QMessageBox::critical(this,"重命名文件","新文件名不能与旧文件一致");
        return;
    }
    if(strNewFileName.isEmpty() ||strNewFileName.toStdString().size()>32)
    {
        QMessageBox::critical(this,"重命名文件","文件名称长度非法");
        return;
    }

    // 将新旧的文件名存放到pdu中
    PDU* pdu = initPDU(m_curPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    memcpy(pdu->caData,strOldFileName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strNewFileName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_curPath.toStdString().c_str(),m_curPath.toStdString().size());
    // 发送重命名请求到服务器
    Client::getInstance().sendPDU(pdu);

}

void File::on_moveFile_PB_clicked()
{
    m_moveFile->m_curPath = m_rootPath;
    // 获取当前选中的列表框
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    // 没选择文件夹
    if(pItem == NULL)
    {
        QMessageBox::information(this,"移动文件","请选择要移动的文件");
        return;
    }
    // 获取选中的文件名
    QString strMoveFileName = pItem->text();
    // 判断是否为文件夹
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        if(pFileInfo->caFileName == strMoveFileName && pFileInfo->uiFileType == ENUM_FILE_TYPE_FOLDER)
        {
            QMessageBox::information(this,"移动文件","请选择文件");
            return;
        }
    }

    // 要移动的文件名
    m_moveFile->m_moveFileName = strMoveFileName;
    // 要移动之前的路径
    m_moveFile->m_beforePath = m_curPath;
    // 如果该界面是隐藏的，则进行展示
    if(m_moveFile->isHidden())
    {
       m_moveFile->setWindowTitle("选择要移动到的路径");
       m_moveFile->setWindowModality(Qt::ApplicationModal);
       m_moveFile->flushFileReq();
       m_moveFile->show();
    }

}

void File::on_uploadFile_PB_clicked()
{
   // 判断是否有文件在上传
   if(m_bUpload)
   {
       QMessageBox::information(this,"上传文件","正在上传文件中，请稍后...");
   }
   // 选择要上传的文件
   m_strUploadFilePath = QFileDialog::getOpenFileName();


   // 获取要上传的文件名
   int index = m_strUploadFilePath.lastIndexOf('/');
   QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.toStdString().size()-index-1);
   // 获取文件名的长度
   int fileNameLen = strFileName.toStdString().size();
   // 获取当前路径长度
   int curPathLen = m_curPath.toStdString().size();

   // 获取文件大小
   QFile file(m_strUploadFilePath);
   qint64 fileSize =  file.size();

   PDU* pdu = initPDU(fileNameLen+curPathLen+1);
   pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
   // 发送文件名长度，文件大小
   memcpy(pdu->caData,&fileNameLen,sizeof(int));
   memcpy(pdu->caData+32,&fileSize,sizeof(qint64));
   // 发送文件名，当前路径
   memcpy(pdu->caMsg,strFileName.toStdString().c_str(),fileNameLen);
   memcpy(pdu->caMsg+fileNameLen,m_curPath.toStdString().c_str(),curPathLen);
   // 测试
   qDebug()<<"File uploadFile fileNameLen"<<fileNameLen;
   qDebug()<<"File uploadFile fileSize"<<fileSize;
   qDebug()<<"File uploadFile strFileName"<<strFileName;
   qDebug()<<"File uploadFile m_curPath"<<m_curPath;
   Client::getInstance().sendPDU(pdu);

}
