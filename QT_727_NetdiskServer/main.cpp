#include "server.h"

#include <QApplication>
#include <operatedb.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w;
    // w.show();

    // 通过创建的实例，调用connect函数，连接数据库
    OperateDB::getInstance().connect();

    return a.exec();
}
