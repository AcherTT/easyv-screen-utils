#include "mysql_pool.h"

MsqlConnectionPool::MsqlConnectionPool(
    Napi::Env *env,
    const char *host,
    const char *user,
    const char *passwd,
    const char *db,
    unsigned int port,
    int pool_size) : host_(host), user_(user), passwd_(passwd), db_(db), port_(port),
                     pool_size_(static_cast<size_t>(pool_size)), current_size_(0), env_(env)

{
    for (size_t i = 0; i < pool_size_; ++i)
    {
        createConnection();
    }
}

void MsqlConnectionPool::createConnection()
{
    MYSQL *conn = mysql_init(nullptr);

    if (!mysql_real_connect(conn, host_, user_, passwd_, db_, port_, nullptr, 0))
    {
        mysql_close(conn);
        Napi::TypeError::New(*env_, "连接创建失败")
            .ThrowAsJavaScriptException();
        return;
    }

    ++current_size_;
    connections_.push(conn);
}

void MsqlConnectionPool::releaseConnection(MYSQL *conn)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if (connections_.size() < pool_size_)
    {
        connections_.push(conn);
    }
    else
    {
        mysql_close(conn);
        --current_size_;
    }

    cv_.notify_one();
}

MsqlConnectionPool::~MsqlConnectionPool()
{
    while (!connections_.empty())
    {
        MYSQL *conn = connections_.front();
        connections_.pop();
        mysql_close(conn);
    }
    delete host_;
    delete user_;
    delete passwd_;
    delete db_;
}

MsqlConnectionPool *MsqlConnectionPool::getConnectPool(
    Napi::Env *env,
    const char *host,
    const char *user,
    const char *passwd,
    const char *db,
    unsigned int port,
    int pool_size)
{
    static MsqlConnectionPool instance(env, host, user, passwd, db, port, pool_size);
    return &instance;
}

// 获取线程池中的连接
MYSQL *MsqlConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);

    while (connections_.empty())
        cv_.wait(lock);

    MYSQL *conn = connections_.front();
    connections_.pop();

    return conn;
}