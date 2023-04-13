#include "pgsql_pool.h"

PgsqlConnectionPool::PgsqlConnectionPool(
    Napi::Env *env, const char *dbUrl, int pool_size)
    : dbUrl_(dbUrl), pool_size_(static_cast<size_t>(pool_size)), current_size_(0), env_(env)
{
    for (int i = 0; i < pool_size; ++i)
        createConnection();
}

PgsqlConnectionPool::~PgsqlConnectionPool()
{
    while (!connections_.empty())
    {
        PGconn *conn = connections_.front();
        connections_.pop();
        PQfinish(conn);
    }

    // 不需要释放这两个指针，因为它们是指向外部的指针
    // delete env_;
    // delete dbUrl_;
}

PGconn *PgsqlConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);

    while (connections_.empty())
        cv_.wait(lock);

    PGconn *conn = connections_.front();
    connections_.pop();

    return conn;
}

void PgsqlConnectionPool::releaseConnection(PGconn *conn)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if (connections_.size() < pool_size_)
    {
        connections_.push(conn);
    }
    else
    {
        PQfinish(conn);
        --current_size_;
    }

    cv_.notify_one();
}

void PgsqlConnectionPool::createConnection()
{
    PGconn *conn = PQconnectdb(dbUrl_);
    if (PQstatus(conn) != CONNECTION_OK)
    {
        PQfinish(conn);
        Napi::TypeError::New(*env_, "连接创建失败")
            .ThrowAsJavaScriptException();
        return;
    }

    ++current_size_;
    connections_.push(conn);
}
