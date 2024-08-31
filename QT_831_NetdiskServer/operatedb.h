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
    // 处理下线的函数
    void handleOffline(const char* name);
    // 处理查找用户的函数
    int handleFindUser(const char* name);
    // 处理在线用户的函数
    QStringList handleOnlineUser();

    // 判断是否转发
    int handleAddFriend(const char *curName,const char* tarName);

    // 建立好友关系
    int handleAddFriendAgree(const char *curName,const char* tarName);

    // 刷新好友
    QStringList handleFlushFriend(const QString& name);

    // 处理删除好友
    int handleDeleteFriend(const QString& name,const char *tarName);

    // 数据库对象
    QSqlDatabase m_db;

private:
    // 将构造函数私有化或删除，防止创建对象
    explicit OperateDB(QObject *parent = nullptr);
    // 删除拷贝构造
    OperateDB(const OperateDB& install) = delete ;
    // 删除拷贝赋值运算符
    OperateDB operator=(const OperateDB&) =delete ;
signals:

};

#endif // OPERATEDB_H
