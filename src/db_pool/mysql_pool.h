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
    static MsqlConnectionPool *getConnectPool(
        Napi::Env *env,
        const char *host,
        const char *user,
        const char *passwd,
        const char *db,
        unsigned int port,
        int pool_size);
    MYSQL *getConnection();
    // 防止外界通过拷贝构造函数和移动拷贝构造函数
    MsqlConnectionPool(const MsqlConnectionPool &obj) = delete;
    MsqlConnectionPool &operator=(const MsqlConnectionPool &obj) = delete;
    ~MsqlConnectionPool();

private:
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