#pragma once

#include <mysql/mysql.h>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <napi.h>

using namespace std;

class MsqlConnectionPool
{
public:
    // 单例模式
    static MsqlConnectionPool *getConnectPool(
        Napi::Env *env,
        const char *host,
        const char *user,
        const char *passwd,
        const char *db,
        unsigned int port,
        int pool_size);
    ~MsqlConnectionPool();
    MYSQL *getConnection();

public:
    // CRUD，因为从查询到获取结果步骤繁琐，所有封装了。
    unique_ptr<MYSQL_RES> *find(MYSQL *conn, string &sql);
    bool update(MYSQL *conn, string &sql);
    u_int64_t insert(MYSQL *conn, string &sql);
    bool del(MYSQL *conn, string &sql);

public:
    // 单例
    MsqlConnectionPool(const MsqlConnectionPool &obj) = delete;
    MsqlConnectionPool &operator=(const MsqlConnectionPool &obj) = delete;

private:
    // 构造函数私有化
    MsqlConnectionPool(
        Napi::Env *env,
        const char *host,
        const char *user,
        const char *passwd,
        const char *db,
        unsigned int port,
        int pool_size);
    void createConnection();             // 生产数据库连接
    void releaseConnection(MYSQL *conn); // 销毁数据库连接

private:
    queue<MYSQL *> connections_;
    mutex mutex_;
    condition_variable cv_;
    const char *host_;
    const char *user_;
    const char *passwd_;
    const char *db_;
    unsigned int port_;
    queue<MYSQL *>::size_type pool_size_;
    queue<MYSQL *>::size_type current_size_;
    Napi::Env *env_;
};