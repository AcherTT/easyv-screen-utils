#pragma once

#include <napi.h>
#include <string>
#include <map>
#include "include/rapidjson/document.h"

using namespace Napi;

#define THROW_JS_ERROR(env, str)                                   \
  {                                                                \
    do                                                             \
    {                                                              \
      Napi::TypeError::New(env, str).ThrowAsJavaScriptException(); \
      return env.Null();                                           \
    } while (0);                                                   \
  }

/**
 * 导入大屏异步任务
 * @param callback 回调函数
 * @param packetPath 包地址
 * @param runTime 超时时间 -- TODO: 未使用
 * 思路：
 * 1. 校验包地址与内容
 * 2. 解压包并获取文件列表
 * 3. 解析大屏包内文件内容
 * 4. 导入大屏数据
 * 5. ...
 */
class ScreenImporterAsyncWorker : public AsyncWorker
{
private:
  int runTime_;              // 超时时间, TODO: 未使用
  std::string packetPath_;   // 包地址-绝对路径
  uint64_t newRootScreenId_; // 执行结果，大屏id

private:                                                                           // 映射关系
  std::map<std::string, rapidjson::Document> screenInfo_;                          // 大屏包内个文件解析出来的内容
  std::map<uint64_t, uint64_t> screenMap_;                                         // 大屏数据
  std::map<uint64_t, uint64_t> componentMap_;                                      // 组件数据
  std::map<uint64_t, uint64_t> filterMap_;                                         // 过滤器数据
  std::map<uint64_t, uint64_t> panelMap_;                                          // 面板数据
  std::map<std::string, std::string> reservePlanIdMap_;                            // 预案数据
  std::map<std::string, std::string> reservePlanCollectionIdMap_;                  // 预案集合数据
  std::map<uint64_t, uint64_t> containerMap_;                                      // 容器数据
  std::map<uint64_t, rapidjson::Document> needImportContainersConfigMap_;          // 容器组件数据
  std::map<uint64_t, uint64_t> individualComponentsMap_;                           // 自定义组件数据
  std::map<std::string, rapidjson::Document> needImportComponentContainerRelsMap_; // 数据容器组件关系数据
  std::map<std::string, std::string> fileMap_;                                     // 包内文件列表

private:                                                     // 解析出来的数据
  rapidjson::Document *componentData_ = nullptr;             // 组件配置文件内容
  rapidjson::Document *componentContainerRelData_ = nullptr; // 数据容器组件关系配置文件内容
  rapidjson::Document *componentCustomInfoData_ = nullptr;   // 自定义组件配置文件内容
  rapidjson::Document *containerData_ = nullptr;             // 容器配置文件内容
  rapidjson::Document *dataSourceData_ = nullptr;            // 数据源配置文件内容
  rapidjson::Document *filterData_ = nullptr;                // 过滤器配置文件内容
  rapidjson::Document *reservePlanData_ = nullptr;           // 预案配置文件内容
  rapidjson::Document *reservePlanCollectionData_ = nullptr; // 预案集合配置文件内容
  rapidjson::Document *panelData_ = nullptr;                 // 面板配置文件内容
  rapidjson::Document *screenData_ = nullptr;                // 大屏配置文件内容

private:                                                  // 工具函数
  void checkPacketPath();                                 // 校验包地址
  void checkPacketValid();                                // 校验包内部是否有对应的文件
  rapidjson::Document *getFileContent(std::string path);  // 获取包文件内容并解析
  rapidjson::Document *getFileContent(char *fileContent); // 获取包文件内容并解析
  std::map<std::string, char *> *unzipGetFiles();         // 解压包并获取文件列表

private:
  void parsePacket();                                   // 解析大屏包内文件内容
  void flatPanel();                                     // 扁平化面板数据
  void importComponents(rapidjson::Document &document); // 导入大屏数据
  void importComponentContainerRel();                   // 导入容器组件关系
  void importContainers();                              // 导入容器
  void importComponentCustomInfo();                     // 导入组件自定义配置
  void importDataSources();                             // 导入数据源
  void importFilters();                                 // 导入过滤器
  void importPanels();                                  // 导入面板
  void importReservePlans();                            // 导入预案
  void importReservePlanCollections();                  // 导入预案集合
  void importRootScreen();                              // 导入大屏信息

  void replaceScreenKey();    // 替换大屏内部的id
  void replaceComponentKey(); // 替换组件内部的id
  void replacePanelKey();     // 替换面板内部的id
  void replaceFilterKey();    // 替换过滤器内部的id
  void cleanDirtyEvents();    // 清理脏数据

public:
  ScreenImporterAsyncWorker(Function &callback, std::string packetPath, int runTime);
  ~ScreenImporterAsyncWorker();
  void Execute(); // 执行异步任务
  void OnOK();    // 执行成功回调
};