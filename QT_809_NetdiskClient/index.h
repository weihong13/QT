#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include "friend.h"
#include "file.h"

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:

    ~Index();
    // 获取单例对象
    static Index& getInstance();

    // 获取好友界面
    Friend* getFriend();
    // 获取文件界面
    File* getFile();

private slots:
    void on_friend_PB_clicked();

    void on_file_PB_clicked();

private:
    explicit Index(QWidget *parent = nullptr);
    Index(const Index& instance) = delete;
    Index operator=(const Index&) = delete;

    Ui::Index *ui;
};

#endif // INDEX_H
