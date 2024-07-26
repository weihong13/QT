#include "operatedb.h"

#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>

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

bool OperateDB::handleRegist(const char* name, const char* pwd)
{
    // 判断参数是否为空指针
    if(name==NULL || pwd==NULL)
    {
        return false;
    }

    // 判断用户是否存在
    // 根据用户名查找用户的语句，利用arg传递变量（'%1' 是占位符）
    QString sql = QString("select * from user_info where name = '%1'").arg(name);
    // 创建一个 QSqlQuery 对象，用来执行 sql语句
    QSqlQuery q;
    // q.exec(sql)执行语句，返回值为是否成功，如果成功了，就去执行q.next()
    // q.next() 是挨个取出 执行成后的每一条结果，如果取到结果了，说明该用户名是存在的
    if(!q.exec(sql)||q.next())
    {
        return false;
    }

    // 插入一条数据
    sql = QString("insert into user_info(name,pwd) values('%1','%2')").arg(name).arg(pwd);
    // 返回执行结果
    return q.exec(sql);
}

bool OperateDB::handleLogin(const char *name, const char *pwd)
{
    // 判断参数是否为空指针
    if(name==NULL || pwd==NULL)
    {
        return false;
    }
    // 判断用户是否存在，密码是否正确
    QString sql = QString("select * from user_info where name = '%1' and pwd = '%2'").arg(name).arg(pwd);
    // 创建一个 QSqlQuery 对象，用来执行 sql语句
    QSqlQuery q;

    // 执行失败，返回false
    if(!q.exec(sql)) return false;
    // q.exec(sql)执行语句，返回值为是否成功，如果成功了，就去执行q.next()
    // q.next() 是挨个取出 执行成后的每一条结果，如果取到结果了，说明该用户名是存在的,密码也正确
    if(!q.exec(sql)||q.next())
    {
        // 用户登录状态改为1
        QString sql = QString("update user_info set online = 1 where name = '%1' and pwd = '%2'").arg(name).arg(pwd);
        if(q.exec(sql))
        {
            return true;
        }
        else
        {
            return false;
        }


    }
    else
    {
        return false;
    }


}

// 处理下线的函数
void OperateDB::handleOffline(const char *name)
{
    // 判断参数是否为空指针
    if(name==NULL)
    {
        return;
    }
    QString sql = QString("update user_info set online = 0 where name = '%1'").arg(name);
    // 创建一个 QSqlQuery 对象，用来执行 sql语句
    QSqlQuery q;
    // 执行更新语句
    q.exec(sql);

}



