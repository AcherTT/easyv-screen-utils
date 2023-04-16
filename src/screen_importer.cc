#include "screen_importer.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zip.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "include/rapidjson/document.h"
#include "include/rapidjson/error/en.h"

ScreenImporterAsyncWorker::ScreenImporterAsyncWorker(Function &callback,
                                                     string packetPath,
                                                     MsqlConnectionPool *pool,
                                                     int runTime = 180)
    : AsyncWorker(callback),
      packetPath_(packetPath),
      pool_(pool),
      runTime_(runTime){};

ScreenImporterAsyncWorker::~ScreenImporterAsyncWorker() {
  if (conn_ != nullptr) {
    pool_->releaseConnection(conn_);
  }
};

void ScreenImporterAsyncWorker::Execute() {
  try {
    conn_ = pool_->getConnection();
    if (conn_ == nullptr) throw runtime_error("数据库连接失败");
    this->checkPacketPath();
    this->parsePacket();
    this->checkPacketValid();

    // 所有数据都准备好了，按流程导入，都存在了this->xxxData_中
    // this->importRootScreen();
    this->newRootScreenId_ = 1;
  } catch (runtime_error &e) {
    cerr << e.what() << endl;
    SetError(e.what());
  }
};

void ScreenImporterAsyncWorker::OnOK() {
  Callback().Call({Env().Null(), Number::New(Env(), this->newRootScreenId_)});
};

unique_ptr<Document> ScreenImporterAsyncWorker::getFileContent(string path) {
  using namespace rapidjson;
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1)
    throw runtime_error(string("failed to open file: ") + strerror(errno));

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    throw runtime_error(string("failed to stat file: ") + strerror(errno));
  }

  char *jsonStr = static_cast<char *>(
      mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (jsonStr == MAP_FAILED) {
    close(fd);
    throw runtime_error(string("failed to mmap file: ") + strerror(errno));
  }

  auto document = make_unique<Document>();
  document->Parse(jsonStr, sb.st_size);

  munmap(jsonStr, sb.st_size);
  close(fd);
  return document;
}

unique_ptr<Document> ScreenImporterAsyncWorker::getFileContent(
    char *fileContent) {
  auto document = make_unique<Document>();
  document->Parse(fileContent);
  auto length = strlen(fileContent);

  if (document->HasParseError()) {
    std::ostringstream errMsg;
    errMsg << "JSON parse errorCode: " << document->GetParseError()
           << " (offset: " << document->GetErrorOffset() << ") "
           << GetParseError_En(document->GetParseError()) << endl;

    throw runtime_error(errMsg.str());
  }

  return document;
}

void ScreenImporterAsyncWorker::checkPacketPath() {
  // 判断路径是否合法
}

void ScreenImporterAsyncWorker::checkPacketValid() {
  using FileCheckPair =
      std::pair<const char *, std::unique_ptr<rapidjson::Document> *>;
  std::array<FileCheckPair, 5> filesToCheck = {{
      {"screenConfig.json", &this->screenData_},
      {"sourceConfig.json", &this->dataSourceData_},
      {"panelConfig.json", &this->panelData_},
      {"filterConfig.json", &this->filterData_},
      {"componentsConfig.json", &this->componentData_},
  }};

  for (const auto &fileCheckPair : filesToCheck) {
    if (fileCheckPair.second->get() == nullptr)
      throw std::runtime_error(std::string("大屏包缺少") + fileCheckPair.first);
  }
}

unique_ptr<map<string, unique_ptr<char[]>>>
ScreenImporterAsyncWorker::unzipGetFiles() {
  int error = 0;
  zip *archive = zip_open(this->packetPath_.c_str(), 0, &error);
  if (!archive)
    throw std::runtime_error("大屏包解压失败: " + std::to_string(error));

  auto files =
      std::make_unique<std::map<std::string, std::unique_ptr<char[]>>>();

  int num_entries = zip_get_num_entries(archive, 0);
  for (int i = 0; i < num_entries; i++) {
    zip_stat_t stat;
    if (zip_stat_index(archive, i, 0, &stat) != 0)
      throw std::runtime_error("文件信息获取失败");

    if (stat.name[strlen(stat.name) - 1] == '/')  // 过滤掉文件夹
      continue;

    if (std::string(stat.name).find(".json") ==
        std::string::npos)  // 过滤掉非json文件
      continue;

    zip_file_t *file = zip_fopen_index(archive, i, 0);
    if (!file)
      throw std::runtime_error(std::string("failed to open file: ") +
                               std::string(stat.name));

    auto buffer = std::make_unique<char[]>(stat.size + 1);

    // 在文件内容末尾添加空字符，方便rapidjson解析，否则会报错
    buffer[stat.size] = '\0';

    if (zip_fread(file, buffer.get(), stat.size) < 0)
      throw std::runtime_error(std::string("failed to read file: ") +
                               std::string(stat.name));

    files->insert({stat.name, std::move(buffer)});
    zip_fclose(file);
  }
  zip_close(archive);
  return files;
}

void ScreenImporterAsyncWorker::parsePacket() {
  using namespace std;
  auto files = this->unzipGetFiles();

  map<string, unique_ptr<Document> *> keywordToDocumentPtr = {
      {"info.json", nullptr},
      {"screenConfig.json", &this->screenData_},
      {"sourceConfig.json", &this->dataSourceData_},
      {"reservePlanConfig.json", &this->reservePlanData_},
      {"reservePlanCollectionConfig.json", &this->reservePlanCollectionData_},
      {"panelConfig.json", &this->panelData_},
      {"filterConfig.json", &this->filterData_},
      {"containersConfig.json", &this->containerData_},
      {"componentsConfig.json", &this->componentData_},
      {"componentCustomInfo.json", &this->componentCustomInfoData_},
      {"componentContainerRelsConfig.json", &this->componentContainerRelData_},
  };

  for (const auto &file : *files) {
    auto document = this->getFileContent(file.second.get());

    for (const auto &entry : keywordToDocumentPtr) {
      if (file.first.find(entry.first) != string::npos) {
        if (entry.first == "info.json") {
          auto version = document->FindMember("version")->value.GetDouble();
          if (version < 4.16) throw runtime_error("大屏包版本不支持");
        } else {
          *entry.second = move(document);
        }
        break;
      }
    }
  }
}

void ScreenImporterAsyncWorker::importRootScreen() {
  // 将大屏包的信息导入到数据库
  auto screenData = this->screenData_->GetObject();
  string query(
      "INSERT INTO dt_easyv_info (`id`, `key`, `value`) values (1, \"test\", "
      "\"test\");");
  // this->pool_->insert(conn_, query);
  string a("select * from dt_easyv_info;");
  this->pool_->find(conn_, a);
}
