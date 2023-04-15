#pragma once

#include <mysql/mysql.h>
#include <napi.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "../include/rapidjson/document.h"

using namespace std;

struct VectorDeleter {
  void operator()(std::vector<rapidjson::Document *> *vec) const {
    for (auto &doc_ptr : *vec) {
      delete doc_ptr;
    }
    delete vec;
  }
};

class MsqlConnectionPool {
 public:
  // 单例模式
  static MsqlConnectionPool *getConnectPool(Napi::Env *env, const string host,
                                            const string user,
                                            const string passwd,
                                            const string db, unsigned int port,
                                            int pool_size);
  ~MsqlConnectionPool();
  MYSQL *getConnection();

 public:
  // CRUD，因为从查询到获取结果步骤繁琐，所有封装了。
  unique_ptr<vector<rapidjson::Document *>, VectorDeleter> find(MYSQL *conn,
                                                                string &sql);
  bool update(MYSQL *conn, string &sql);
  u_int64_t insert(MYSQL *conn, string &sql);
  bool del(MYSQL *conn, string &sql);

 public:
  // 单例
  MsqlConnectionPool(const MsqlConnectionPool &obj) = delete;
  MsqlConnectionPool &operator=(const MsqlConnectionPool &obj) = delete;

 private:
  // 构造函数私有化
  MsqlConnectionPool(Napi::Env *env, const string host, const string user,
                     const string passwd, const string db, unsigned int port,
                     int pool_size);
  void createConnection();              // 生产数据库连接
  void releaseConnection(MYSQL *conn);  // 销毁数据库连接

 private:
  queue<MYSQL *> connections_;
  mutex mutex_;
  condition_variable cv_;
  const string host_;
  const string user_;
  const string passwd_;
  const string db_;
  unsigned int port_;
  queue<MYSQL *>::size_type pool_size_;
  queue<MYSQL *>::size_type current_size_;
  Napi::Env *env_;
};