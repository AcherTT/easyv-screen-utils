#include "mysql_pool.h"
#include <iostream>

using namespace std;

MsqlConnectionPool::MsqlConnectionPool(Napi::Env *env, const string host,
                                       const string user, const string passwd,
                                       const string db, unsigned int port,
                                       int pool_size)
    : host_(host), user_(user), passwd_(passwd), db_(db), port_(port),
      pool_size_(static_cast<size_t>(pool_size)), current_size_(0), env_(env)

{
  for (size_t i = 0; i < pool_size_; ++i) {
    createConnection();
  }
}

void MsqlConnectionPool::createConnection() {
  MYSQL *conn = new MYSQL;
  mysql_init(conn);
  if (!mysql_real_connect(conn, host_.c_str(), user_.c_str(), passwd_.c_str(),
                          db_.c_str(), port_, nullptr, 0)) {
    mysql_close(conn);
    Napi::TypeError::New(*env_, "连接创建失败").ThrowAsJavaScriptException();
    return;
  }

  ++current_size_;
  connections_.push(conn);
}

void MsqlConnectionPool::releaseConnection(MYSQL *conn) {
  unique_lock<mutex> lock(mutex_);

  if (connections_.size() < pool_size_) {
    connections_.push(conn);
  } else {
    mysql_close(conn);
    --current_size_;
  }

  cv_.notify_one();
}

MsqlConnectionPool::~MsqlConnectionPool() {
  while (!connections_.empty()) {
    MYSQL *conn = connections_.front();
    connections_.pop();
    mysql_close(conn);
  }
}

MsqlConnectionPool *MsqlConnectionPool::getConnectPool(
    Napi::Env *env, const string host, const string user, const string passwd,
    const string db, unsigned int port, int pool_size) {
  static MsqlConnectionPool instance(env, host, user, passwd, db, port,
                                     pool_size);
  return &instance;
}

// 获取线程池中的连接
MYSQL *MsqlConnectionPool::getConnection() {
  unique_lock<mutex> lock(mutex_);

  while (connections_.empty())
    cv_.wait(lock);

  MYSQL *conn = connections_.front();
  connections_.pop();

  return conn;
}

unique_ptr<vector<rapidjson::Document *>, VectorDeleter>
MsqlConnectionPool::find(MYSQL *conn, string &sql) {
  using namespace rapidjson;
  if (mysql_query(conn, sql.c_str()))
    throw runtime_error(string("查询失败： ") + string(mysql_error(conn)));

  MYSQL_RES *queryResult = mysql_store_result(conn);
  if (!queryResult)
    throw runtime_error(string("查询结果读取失败： ") +
                        string(mysql_error(conn)));

  unique_ptr<vector<Document *>, VectorDeleter> results(new vector<Document *>);
  unsigned int num_fields = mysql_num_fields(queryResult);
  MYSQL_FIELD *fields = mysql_fetch_fields(queryResult);
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(queryResult))) {
    auto result = new Document();
    result->SetObject(); // 确保 Document 是一个对象类型
    for (int i = 0; i < num_fields; i++) {
      result->AddMember(
          Value(fields[i].name, strlen(fields[i].name), result->GetAllocator()),
          Value(row[i], strlen(row[i]), result->GetAllocator()),
          result->GetAllocator());
    }
    results->push_back(result);
  }
  mysql_free_result(queryResult);
  return results;
}

bool MsqlConnectionPool::update(MYSQL *conn, string &sql) {
  if (mysql_query(conn, sql.c_str()))
    throw runtime_error(string("更新失败： ") + string(mysql_error(conn)));

  return true;
}

u_int64_t MsqlConnectionPool::insert(MYSQL *conn, string &sql) {
  auto result = mysql_query(conn, sql.c_str());
  if (result)
    throw runtime_error(string("插入失败： ") + string(mysql_error(conn)));

  return mysql_insert_id(conn);
}

bool MsqlConnectionPool::del(MYSQL *conn, string &sql) {
  if (mysql_query(conn, sql.c_str()))
    throw runtime_error("删除失败");
  return true;
}
