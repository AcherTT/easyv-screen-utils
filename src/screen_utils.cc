#include "screen_utils.h"
#include "screen_importer.h"
#include "db_pool.h"

using namespace Napi;

ScreenImportUtils::ScreenImportUtils(const Napi::CallbackInfo &info)
    : ObjectWrap(info)
{
  Napi::Env env = info.Env();

  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "不能没有参数哇").ThrowAsJavaScriptException();
    return;
  }

  if (!info[0].IsString())
  {
    Napi::TypeError::New(env, "参数不对哇").ThrowAsJavaScriptException();
    return;
  }

  this->dbUrl_ = info[0].As<Napi::String>().Utf8Value();
  this->pool_ = new PgsqlConnectionPool(&env, this->dbUrl_.c_str(), 4); // 创建连接池
}

ScreenImportUtils::~ScreenImportUtils()
{
  delete this->pool_;
}

Napi::Value ScreenImportUtils::ImportScreen(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() < 3)
    THROW_JS_ERROR(env, "参数数量不对哇");

  if (!info[0].IsString())
    THROW_JS_ERROR(env, "参数不对哇-包路径");

  if (!info[1].IsFunction())
    THROW_JS_ERROR(env, "参数不对哇-回调函数");

  if (!info[2].IsNumber())
    THROW_JS_ERROR(env, "参数不对哇-超时时间");

  String packetPath = info[0].As<String>();
  Function callback = info[1].As<Function>();
  int runTime = info[2].As<Number>();

  ScreenImporterAsyncWorker *asyncWorker = new ScreenImporterAsyncWorker(callback, packetPath, runTime);
  asyncWorker->Queue();

  return String::New(info.Env(), "OK");
}

Napi::Function ScreenImportUtils::GetClass(Napi::Env env)
{
  return DefineClass(
      env,
      "ScreenImportUtils",
      {
          ScreenImportUtils::InstanceMethod("importScreen", &ScreenImportUtils::ImportScreen),
      });
}
