#ifndef INDEX_H
#define INDEX_H

#include <QWidget>

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    explicit Index(QWidget *parent = nullptr);
    ~Index();

private slots:
    void on_friend_PB_clicked();

    void on_file_PB_clicked();

private:
    Ui::Index *ui;
};

#endif // INDEX_H
