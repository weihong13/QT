#include "client.h"
#include "selectpath.h"
#include "ui_selectpath.h"

#include <QInputDialog>
#include <QMessageBox>

SelectPath::SelectPath(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectPath)
{
    // 获取用户根目录
    m_rootPath = QString("%1/%2")
            .arg(Client::getInstance().getRootPath())
            .arg(Client::getInstance().getLoginName());
    m_curPath = m_rootPath;

    // 主窗口关闭，移动文件窗口也会跟着关闭
    setAttribute(Qt::WA_QuitOnClose,false);

    ui->setupUi(this);
}

SelectPath::~SelectPath()
{
    delete ui;
}

// 刷新文件列表
void SelectPath::flushFileReq()
{
    // 发送当前路径给服务器
    PDU* pdu = initPDU(m_curPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SELECT_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_curPath.toStdString().c_str(),m_curPath.toStdString().size());
    qDebug()<<"SelectPath flushFileReq pdu->caMsg"<< pdu->caMsg;
    Client::getInstance().sendPDU(pdu);
}

void SelectPath::updateFileList(QList<FileInfo *> fileList)
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
    ui->selectPath_LW->clear();
    // 将fileList中每个文件的信息挨个取出
    foreach(FileInfo* pFileInfo,fileList)
    {
        QListWidgetItem* pItem = new QListWidgetItem;
        // 设置文件名
        pItem->setText(pFileInfo->caFileName);
        // 设置文件图标
        if(pFileInfo->uiFileType == ENUM_FILE_TYPE_FOLDER)
        {
            pItem->setIcon(QIcon(QPixmap(":/fileIcon/folder.png")));
        }
        else if(pFileInfo->uiFileType == ENUM_FILE_TYPE_TXT)
        {
            pItem->setIcon(QIcon(QPixmap(":/:/fileIcon/txt.png")));
        }
        ui->selectPath_LW->addItem(pItem);
    }
    // 展示新的文件列表框的内容
    ui->selectPath_LW->show();
}

// 创建文件夹
void SelectPath::on_mkdir_PB_clicked()
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
    // 将当前路径 和 要创建的文件夹名称存入pdu
    memcpy(pdu->caData,strFolderName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,curFileName.toStdString().c_str(),curFileName.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}

// 返回上一级
void SelectPath::on_return_PB_clicked()
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

// 确认保存位置
void SelectPath::on_confirm_PB_clicked()
{
    // 获取当前选中的列表框
    QListWidgetItem* pItem = ui->selectPath_LW->currentItem();
    // 没选择内容
    if(pItem == NULL)
    {
        QMessageBox::information(this,"保存文件","请选择要保存到的目录");
        return;
    }
    // 获取选中的文件夹名称
    QString strCurFolder = pItem->text();
    // 判断是否为文件夹
    foreach(FileInfo* pFileInfo,m_fileInfoList)
    {
        if(pFileInfo->caFileName == strCurFolder && pFileInfo->uiFileType != ENUM_FILE_TYPE_FOLDER)
        {
            QMessageBox::information(this,"保存文件","请选择文件夹");
            return;
        }
    }

    // 文件要保存到的文件夹
    QString strSavePath = QString("%1/%2").arg(m_curPath).arg(strCurFolder);

    // 再次确认，是否要保存到该路径下
    int ret = QMessageBox::No;
    ret = QMessageBox::question(this,"保存文件",QString("是否将文件保存到该目录：%1").arg(strSavePath));
    // 不移动
    if(ret == QMessageBox::No)
    {
        return;
    }
    // 文件的完整的保存后路径 添加文件名
    QString strTarFilePath = QString("%1/%2").arg(strSavePath).arg(m_fileName);

    qDebug()<<"SelectPath confirm_PB strTarFilePath"<<strTarFilePath;

    // 计算 路径长度
    int srcLen = m_sharePath.toStdString().size();
    int tarLen = strTarFilePath.toStdString().size();
    // 构建pdu
    PDU* pdu = initPDU(srcLen+tarLen);

    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_ARGEE_REQUEST;

    memcpy(pdu->caData,&srcLen,sizeof(int));
    memcpy(pdu->caData+32,&tarLen,sizeof(int));
    memcpy(pdu->caMsg,m_sharePath.toStdString().c_str(),srcLen);
    memcpy(pdu->caMsg+srcLen,strTarFilePath.toStdString().c_str(),tarLen);

    Client::getInstance().sendPDU(pdu);


}

void SelectPath::on_cancel_PB_clicked()
{
    this->close();
}

// 双击文件夹进入下一级目录
void SelectPath::on_selectPath_LW_itemDoubleClicked(QListWidgetItem *item)
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
