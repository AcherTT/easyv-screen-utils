#include "screen_importer.h"
#include <string>
#include "include/rapidjson/document.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>
// #include <fstream>
#include <zip.h>

ScreenImporterAsyncWorker::ScreenImporterAsyncWorker(
    Function &callback,
    std::string packetPath,
    int runTime = 180) : AsyncWorker(callback), packetPath_(packetPath), runTime_(runTime){};

ScreenImporterAsyncWorker::~ScreenImporterAsyncWorker()
{
  // 释放内存
  if (componentData_ != nullptr)
    delete componentData_;
  if (componentContainerRelData_ != nullptr)
    delete componentContainerRelData_;
  if (componentCustomInfoData_ != nullptr)
    delete componentCustomInfoData_;
  if (containerData_ != nullptr)
    delete containerData_;
  if (dataSourceData_ != nullptr)
    delete dataSourceData_;
  if (filterData_ != nullptr)
    delete filterData_;
  if (reservePlanData_ != nullptr)
    delete reservePlanData_;
  if (reservePlanCollectionData_ != nullptr)
    delete reservePlanCollectionData_;
  if (screenData_ != nullptr)
    delete screenData_;
  if (panelData_ != nullptr)
    delete panelData_;
};

void ScreenImporterAsyncWorker::Execute()
{
  try
  {
    this->checkPacketPath();
    this->parsePacket();
    this->checkPacketValid();

    // 所有数据都准备好了，按流程导入，都存在了this->xxxData_中
    this->importRootScreen();
  }
  catch (std::runtime_error &e)
  {
    std::cerr << e.what() << std::endl;
    SetError(e.what());
  }
};

void ScreenImporterAsyncWorker::OnOK()
{
  Callback().Call({Env().Null(), Number::New(Env(), this->newRootScreenId_)});
};

rapidjson::Document *ScreenImporterAsyncWorker::getFileContent(std::string path)
{
  using namespace rapidjson;
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1)
    throw std::runtime_error(std::string("failed to open file: ") + std::strerror(errno));

  struct stat sb;
  if (fstat(fd, &sb) == -1)
  {
    close(fd);
    throw std::runtime_error(std::string("failed to stat file: ") + std::strerror(errno));
  }

  char *jsonStr = static_cast<char *>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (jsonStr == MAP_FAILED)
  {
    close(fd);
    throw std::runtime_error(std::string("failed to mmap file: ") + std::strerror(errno));
  }

  Document *document = new Document();
  document->Parse(jsonStr);

  munmap(jsonStr, sb.st_size);
  close(fd);
  return document;
}

rapidjson::Document *ScreenImporterAsyncWorker::getFileContent(char *fileContent)
{
  rapidjson::Document *document = new rapidjson::Document();
  document->Parse(fileContent);
  delete[] fileContent;
  return document;
}

void ScreenImporterAsyncWorker::checkPacketPath()
{
  // 判断路径是否合法
  // try
  // {
  //   std::filesystem::path path(packetPath_);
  // }
  // catch (std::filesystem::filesystem_error &e)
  // {
  //   throw std::runtime_error("大屏包路径不合法");
  // }
  // std::cout << "packetPath_: " << packetPath_ << std::endl;
  // // 判断文件是否存在
  // if (!std::filesystem::exists(packetPath_))
  //   throw std::runtime_error("大屏包不存在或者不可读");
}

void ScreenImporterAsyncWorker::checkPacketValid()
{
  // 校验几个必要的json文件是否存在
  if (this->screenData_ == nullptr)
    throw std::runtime_error("大屏包缺少screenConfig.json");
  if (this->dataSourceData_ == nullptr)
    throw std::runtime_error("大屏包缺少sourceConfig.json");
  if (this->panelData_ == nullptr)
    throw std::runtime_error("大屏包缺少panelConfig.json");
  if (this->filterData_ == nullptr)
    throw std::runtime_error("大屏包缺少filterConfig.json");
  if (this->componentData_ == nullptr)
    throw std::runtime_error("大屏包缺少componentsConfig.json");
}

std::map<std::string, char *> *ScreenImporterAsyncWorker::unzipGetFiles()
{
  int error = 0;
  zip *archive = zip_open(this->packetPath_.c_str(), 0, &error);
  if (!archive)
    throw std::runtime_error("大屏包解压失败: " + std::to_string(error));
  std::map<std::string, char *> *files = new std::map<std::string, char *>();

  int num_entries = zip_get_num_entries(archive, 0);
  for (int i = 0; i < num_entries; i++)
  {
    zip_stat_t stat;
    if (zip_stat_index(archive, i, 0, &stat) != 0)
      throw std::runtime_error("文件信息获取失败");

    if (stat.name[strlen(stat.name) - 1] == '/') // 过滤掉文件夹
      continue;

    if (std::string(stat.name).find(".json") == std::string::npos) // 过滤掉非json文件
      continue;

    zip_file_t *file = zip_fopen_index(archive, i, 0);
    if (!file)
      throw std::runtime_error(std::string("failed to open file: ") + std::string(stat.name));

    char *buffer = new char[stat.size];
    if (zip_fread(file, buffer, stat.size) < 0)
      throw std::runtime_error(std::string("failed to read file: ") + std::string(stat.name));

    files->insert(std::pair<std::string, char *>(stat.name, buffer));
  }
  return files;
}

void ScreenImporterAsyncWorker::parsePacket()
{
  using namespace std;
  auto files = this->unzipGetFiles();
  for (auto file : *files)
  {
    rapidjson::Document *document = this->getFileContent(file.second);
    // 头疼，原生c++自己没有反射机制，只能一个一个写if了, switch也不行
    if (file.first.find("info.json") != string::npos)
    {
      auto version = document->FindMember("version")->value.GetDouble();
      if (version < 4.16)
        throw std::runtime_error("大屏包版本不支持");
    }
    else if (file.first.find("screenConfig.json") != string::npos)
      this->screenData_ = document;
    else if (file.first.find("sourceConfig.json") != string::npos)
      this->dataSourceData_ = document;
    else if (file.first.find("reservePlanConfig.json") != string::npos)
      this->reservePlanData_ = document;
    else if (file.first.find("reservePlanCollectionConfig.json") != string::npos)
      this->reservePlanCollectionData_ = document;
    else if (file.first.find("panelConfig.json") != string::npos)
      this->panelData_ = document;
    else if (file.first.find("filterConfig.json") != string::npos)
      this->filterData_ = document;
    else if (file.first.find("containersConfig.json") != string::npos)
      this->containerData_ = document;
    else if (file.first.find("componentsConfig.json") != string::npos)
      this->componentData_ = document;
    else if (file.first.find("componentCustomInfo.json") != string::npos)
      this->componentCustomInfoData_ = document;
    else if (file.first.find("componentContainerRelsConfig.json") != string::npos)
      this->componentContainerRelData_ = document;
  }
  delete files;
}

void ScreenImporterAsyncWorker::importRootScreen()
{
  this->newRootScreenId_ = 111;
}
