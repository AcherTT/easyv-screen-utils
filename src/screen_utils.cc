#include "screen_utils.h"

#include "screen_importer.h"

using namespace Napi;

ScreenImportUtils::ScreenImportUtils(const Napi::CallbackInfo &info)
    : ObjectWrap(info) {
  Napi::Env env = info.Env();

  if (info.Length() < 5) {
    Napi::TypeError::New(env, "不能没有参数哇").ThrowAsJavaScriptException();
    return;
  }

  for (int i = 0; i < 4; ++i) {
    if (!info[i].IsString()) {
      Napi::TypeError::New(env, "参数不对哇").ThrowAsJavaScriptException();
      return;
    }
  }

  if (!info[4].IsNumber()) {
    Napi::TypeError::New(env, "参数不对哇").ThrowAsJavaScriptException();
    return;
  }

  auto host = info[0].As<Napi::String>().Utf8Value();
  auto user = info[1].As<Napi::String>().Utf8Value();
  auto passwd = info[2].As<Napi::String>().Utf8Value();
  auto db = info[3].As<Napi::String>().Utf8Value();
  auto port = info[4].As<Napi::Number>().Int32Value();
  this->pool_ = MsqlConnectionPool::getConnectPool(
      &env, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, 4);
}

ScreenImportUtils::~ScreenImportUtils() {}

Napi::Value ScreenImportUtils::ImportScreen(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 3) THROW_JS_ERROR(env, "参数数量不对哇");

  if (!info[0].IsString()) THROW_JS_ERROR(env, "参数不对哇-包路径");

  if (!info[1].IsFunction()) THROW_JS_ERROR(env, "参数不对哇-回调函数");

  if (!info[2].IsNumber()) THROW_JS_ERROR(env, "参数不对哇-超时时间");

  String packetPath = info[0].As<String>();
  Function callback = info[1].As<Function>();
  int runTime = info[2].As<Number>();

  ScreenImporterAsyncWorker *asyncWorker =
      new ScreenImporterAsyncWorker(callback, packetPath, pool_, runTime);
  asyncWorker->Queue();

  return String::New(info.Env(), "OK");
}

Napi::Function ScreenImportUtils::GetClass(Napi::Env env) {
  return DefineClass(env, "ScreenImportUtils",
                     {
                         ScreenImportUtils::InstanceMethod(
                             "importScreen", &ScreenImportUtils::ImportScreen),
                     });
}
