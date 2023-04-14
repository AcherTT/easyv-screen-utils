#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <libpq-fe.h>
#include <napi.h>

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
    std::queue<PGconn *>::size_type pool_size_;
    std::queue<PGconn *>::size_type current_size_;
    Napi::Env *env_;

private:
    void createConnection();
};