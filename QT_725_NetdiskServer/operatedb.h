#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class OperateDB : public QObject
{
    Q_OBJECT
public:

    ~OperateDB();

    // 连接数据库
    void connect();

    // 获取数据库的单例对象
    static OperateDB& getInstance();

    // 处理注册请求
    bool handleRegist(const char* name,const char* pwd);

    // 处理登录请求
    bool handleLogin(const char* name,const char* pwd);

    // 数据库对象
    QSqlDatabase m_db;

private:
    // 将构造函数私有化或删除，防止创建对象
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& install) = delete ;
    OperateDB operator=(const OperateDB&) =delete ;
signals:

};

#endif // OPERATEDB_H
