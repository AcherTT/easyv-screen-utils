#include "screen_utils.h"
#include "screen_importer.h"

using namespace Napi;

ScreenImportUtils::ScreenImportUtils(const Napi::CallbackInfo &info)
    : ObjectWrap(info)
{
  Napi::Env env = info.Env();

  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "不能没有参数哇")
        .ThrowAsJavaScriptException();
    return;
  }

  if (!info[0].IsString())
  {
    Napi::TypeError::New(env, "参数不对哇")
        .ThrowAsJavaScriptException();
    return;
  }

  this->dbUrl = info[0].As<Napi::String>().Utf8Value();
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

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Napi::String name = Napi::String::New(env, "ScreenImportUtils");
  exports.Set(name, ScreenImportUtils::GetClass(env));
  return exports;
}

NODE_API_MODULE(addon, Init)