#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = nullptr);
    ~ShareFile();

    // 展示好友列表
    void showFriend_LW();
    // 要分享的文件名
    QString m_shareFileName;
    // 要分享文件的路径
    QString m_sharPath;

private slots:
    void on_selectAll_PB_clicked();

    void on_cancelSelectAll_PB_clicked();

    void on_ok_PB_clicked();

private:
    Ui::ShareFile *ui;
};

#endif // SHAREFILE_H
