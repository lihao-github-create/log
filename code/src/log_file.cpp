#include "log_file.h"
#include "append_file.h"
#include "mutex_macro.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <sys/time.h>
#include <thread>
#include <time.h>
#include <unistd.h>

using std::atomic;
using std::mutex;
using std::string;
using std::thread;
using std::unique_ptr;

namespace log {

string getHostName();

class LogFile::Impl {
public:
  Impl(const string &basename, int rollSize, int flushInterval);
  ~Impl();

  void append(const char *logline, size_t len);
  void flush();
  void rollFile();

private:
  string getLogFileName(const string &basename);
  void flushThreadFunc();

private:
  const string basename_;
  const int rollSize_;
  const int flushInterval_;
  mutex mutex_;
  unique_ptr<AppendFile> file_ GUARDED_BY(mutex_);
  thread flushThread_;
  atomic<bool> running_;
};

LogFile::Impl::Impl(const string &basename, int rollSize, int flushInterval)
    : basename_(basename), rollSize_(rollSize), flushInterval_(flushInterval),
      file_(new AppendFile(basename)) {
  flushThread_ = thread([&]() {
    running_ = true;
    flushThreadFunc();
  });
}

LogFile::Impl::~Impl() {
  running_ = false;
  flushThread_.join();
}

void LogFile::Impl::append(const char *logline, size_t len) {
  std::lock_guard<mutex> lock(mutex_);
  file_->append(logline, len);
  if (file_->writtenBytes() > rollSize_) {
    rollFile();
  }
}

void LogFile::Impl::flush() {
  std::lock_guard<mutex> lock(mutex_);
  file_->flush();
}

void LogFile::Impl::rollFile() {
  string filename = getLogFileName(basename_);
  {
    std::lock_guard<mutex> lock(mutex_);
    file_.reset(new AppendFile(basename_));
  }
}

string LogFile::Impl::getLogFileName(const string &basename) {
  string filename;
  filename.reserve(basename.size() + 64);
  // basename
  filename = basename;
  // time
  char timebuf[32] = {0};
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  struct tm *timeInfo = gmtime(&tv.tv_sec);
  snprintf(timebuf, sizeof(timebuf), ".%4d%02d%02d-%02d%02d%02d(UTC).",
           timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
           timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  filename += timebuf;
  // hostname
  filename += getHostName();
  // pid
  char pidbuf[32] = {0};
  snprintf(pidbuf, sizeof pidbuf, ".%d", getpid());
  filename += pidbuf;
  // .log
  filename += ".log";

  return filename;
}

void LogFile::Impl::flushThreadFunc() {
  while (running_) {
    std::this_thread::sleep_for(std::chrono::seconds(flushInterval_));
    flush();
  }
}

LogFile::LogFile(const string &basename, int rollSize, int flushInterval = 3)
    : impl_(new Impl(basename, rollSize, flushInterval)) {}
LogFile::~LogFile() {}

void LogFile::append(const char *logline, size_t len) {
  impl_->append(logline, len);
}

void LogFile::flush() { impl_->flush(); }

string getHostName() {
  char buf[256] = {0};
  if (gethostname(buf, sizeof buf) == 0) {
    return buf;
  } else {
    return "unknownhost";
  }
}

} // namespace log