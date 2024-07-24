#include "operatedb.h"

#include <QDebug>
#include <QSqlError>
#include <QMessageBox>

// 构造函数
OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    // 添加数据库，并使用成员变量接收
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

// 析构函数
OperateDB::~OperateDB()
{
    // 关闭数据库
    m_db.close();
}

// 连接数据库的函数
void OperateDB::connect()
{
    // 添加数据库的信息
    m_db.setHostName("localhost"); // 主机名-本机
    m_db.setPort(3306);            // 端口号
    m_db.setDatabaseName("NetdiskServer"); // 数据库名称
    m_db.setUserName("root");      // 用户名
    m_db.setPassword("171223");    // 密码

    if(m_db.open()) // 连接成功的话，进行提示
    {
        qDebug()<<"数据库连接成功！";
    }
    else
    {   // 连接失败，提示错误信息。
        QMessageBox::critical(0,"DataBase Error",m_db.lastError().text());
    }

}

// 获取数据库的单例对象
OperateDB& OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}



