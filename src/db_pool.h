#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <libpq-fe.h>

class PgsqlConnectionPool
{
public:
    PgsqlConnectionPool(Napi::Env *env, const char *dbUrl, int pool_size);
    ~PgsqlConnectionPool();
    PGconn *getConnection();
    void releaseConnection(PGconn *conn);

private:
    std::queue<PGconn *> connections_;
    std::mutex mutex_;
    std::condition_variable cv_;
    const char *dbUrl_;
    int pool_size_;
    int current_size_;
    Napi::Env *env_;

private:
    void createConnection();
};