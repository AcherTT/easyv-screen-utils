#include "screen_utils.h"
#include <napi.h>

using namespace Napi;

Object Init(Env env, Object exports)
{
    String name = String::New(env, "ScreenImportUtils");
    exports.Set(name, ScreenImportUtils::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
