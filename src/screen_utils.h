#pragma once

#include <napi.h>

/**
 * @brief ScreenImportUtils class
 * 大屏导入工具 C++ 实现
 */
class ScreenImportUtils : public Napi::ObjectWrap<ScreenImportUtils> {
 public:
  ScreenImportUtils(const Napi::CallbackInfo&);
  Napi::Value ImportScreen(const Napi::CallbackInfo&);

  static Napi::Function GetClass(Napi::Env);

 private:
  std::string dbUrl;
};