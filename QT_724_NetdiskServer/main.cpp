#include "server.h"

#include <QApplication>
#include <operatedb.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w;
    w.show();
    OperateDB::getInstance().connect();

    return a.exec();
}
