#pragma once

#include <napi.h>
#include <string>
#include "include/rapidjson/document.h"

using namespace Napi;

#define THROW_JS_ERROR(env, str) { \
  do { \
    Napi::TypeError::New(env, str).ThrowAsJavaScriptException(); \
    return env.Null(); \
  } while (0); \
}

class ScreenImporterAsyncWorker : public AsyncWorker
{
private:
  int runTime;            // 超时时间, TODO: 未使用
  std::string packetPath; // 包地址-绝对路径

private:
  bool checkPacketPath(); // 校验包地址
  bool checkPacketExist();   // 校验包是否存在
  bool checkPacketValid();   // 校验包是否合法
  rapidjson::Document getFileContent(std::string path);    // 获取包文件内容
  void importScreen();       // 导入大屏

public:
  ScreenImporterAsyncWorker(Function &callback, std::string packetPath, int runTime = 100);
  virtual ~ScreenImporterAsyncWorker(){};

  void Execute(); // 执行异步任务
  void OnOK(); // 执行成功回调
};