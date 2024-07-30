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
    // 返回静态局部引用对象，防止调用拷贝构造，静态变量只能赋值一次
    // 在C++11之后，默认支持线程安全
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

// 处理登录的函数
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

    // q.exec(sql)执行语句，返回值为是否成功，如果成功了，就去执行q.next()
    // q.next() 如果取到结果了，说明该用户名是存在的,密码也正确
    if(q.exec(sql)&&q.next())
    {
        // 用户登录状态改为1
        QString sql = QString("update user_info set online = 1 where name = '%1' and pwd = '%2'").arg(name).arg(pwd);
        if(q.exec(sql))
        {
            // 修改成功，返回真
            return true;
        }
        else
        {   // 修改失败，返回错误
            return false;
        }


    }
    else
    {   // 执行失败，或没取到结果，返回错误
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

// 处理查找用户的函数
int OperateDB::handleFindUser(const char *name)
{
    if(name==NULL)
    {
        return -2; // 空指针 返回-2
    }
    // 判断用户是否存在，输出在线状态
    QString sql = QString("select online from user_info where name = '%1'").arg(name);
    // 创建一个 QSqlQuery 对象，用来执行 sql语句
    QSqlQuery q;
    if(!q.exec(sql))
    {
        return -2; // 执行错误，返回-2
    }
    // q.next() 取到结果集的一条数据
    if(q.next())
    {
        // 返回一条数据，可能有多个字段，需要传入一个数，选取第几个字段
        //查找到内容，返回在线状态，需要获取的值为字符串类型，得转换为整型
        return q.value(0).toInt();
    }
    else
    {
        // 没查到内容，用户不存在
        return -1; // 用户不存在
    }
}

 // 处理在线用户的函数
QStringList OperateDB::handleOnlineUser()
{
    // 判断用户是否在线，输出用户名
    QString sql = QString("select name from user_info where online = 1");
    // 创建一个 QSqlQuery 对象，用来执行 sql语句
    QSqlQuery q;
    q.exec(sql);

    QStringList nameList;
    while(q.next())
    {
        // 测试--打印从数据库取到的每一条用户名
        // qDebug()<<"handleOnlineUser "<<q.value(0).toString();
        // 将在线的用户名添加到列表中
        nameList.append(q.value(0).toString());
    }
    return nameList;

}



