#include "screen_importer.h"
#include <chrono>
#include <thread>
#include <string>
#include "include/rapidjson/filereadstream.h"
#include "include/rapidjson/document.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ScreenImporterAsyncWorker::ScreenImporterAsyncWorker(
    Function &callback, std::string packetPath, int runTime)
    : AsyncWorker(callback), packetPath(packetPath), runTime(runTime){};

void ScreenImporterAsyncWorker::Execute()
{
  std::this_thread::sleep_for(std::chrono::seconds(runTime));
  this->checkPacketPath();
  this->checkPacketExist();
  this->checkPacketValid();
  this->importScreen();
};

void ScreenImporterAsyncWorker::OnOK()
{
  std::string msg = "ScreenImporterAsyncWorker returning after 'working' " +
                    std::to_string(runTime) + " seconds.";
  Callback().Call({Env().Null(), String::New(Env(), msg)});
};

/**
 * 使用mmap读取文件内容，因为文件可能比较大，mmap性能和内存占用更好
 */
rapidjson::Document ScreenImporterAsyncWorker::getFileContent(std::string path)
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

  Document document;
  document.Parse(jsonStr);

  munmap(jsonStr, sb.st_size);
  close(fd);
  return document;
}

bool ScreenImporterAsyncWorker::checkPacketPath()
{
  return true;
}

bool ScreenImporterAsyncWorker::checkPacketExist()
{
  return true;
}

bool ScreenImporterAsyncWorker::checkPacketValid()
{
  return true;
}

void ScreenImporterAsyncWorker::importScreen()
{
}
