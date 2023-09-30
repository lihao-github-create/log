#include "logger.h"
#include "current_thread.h"
#include "log_stream.h"
#include <sys/time.h>
#include <thread>
#include <time.h>

namespace log {
constexpr uint32_t NUM_LOG_LEVELS = 6U;
const char *LogLevelName[NUM_LOG_LEVELS] = {
    "TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL ",
};
/********************************Logger::Impl*************************************/
class Logger::Impl {
public:
  Impl(enum LogLevel level, const char *file, int line);

  void formatTime();
  void finish();

  LogStream &stream() { return stream_; }

  struct timeval currentTime_;
  LogStream stream_;
  enum LogLevel level_;
  int line_;
  const char *file_;
};

Logger::Impl::Impl(enum LogLevel level, const char *file, int line)
    : stream_(), level_(level), file_(file), line_(line) {
  formatTime();
  currentthread::tid();
  stream_ << StringPiece(currentthread::tidString(),
                         currentthread::tidStringLength());
  stream_ << LogLevelName[static_cast<uint32_t>(level_)];
}

void Logger::Impl::formatTime() {
  gettimeofday(&currentTime_, NULL);
  // 获取秒数和微秒数
  time_t seconds = currentTime_.tv_sec;
  suseconds_t microseconds = currentTime_.tv_usec;

  // 使用秒数创建 struct timeval 结构体
  struct timeval tv;
  tv.tv_sec = seconds;
  tv.tv_usec = microseconds;

  // 使用 gmtime 函数将秒数转换为 struct tm 结构体（UTC 时间）
  struct tm *timeInfo = gmtime(&seconds);

  // 输出当前时间到 stream_中
  char t_time[64] = {0};
  snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d.06d(UTC) \n",
           timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
           timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, microseconds);
  stream_ << t_time;
}

void Logger::Impl::finish() {
  stream_ << " - " << file_ << ':' << line_ << '\n';
}

/**********************************Logger*********************************/
void defaultOutput(const char *msg, int len) {
  size_t n = fwrite(msg, 1, len, stdout);
}

void defaultFlush() { fflush(stdout); }

LogLevel Logger::level_ = LogLevel::INFO;
OutputFunc Logger::outputFunc_ = defaultOutput;
FlushFunc Logger::flushFunc_ = defaultFlush;

Logger::Logger(const char *file, int line, LogLevel level)
    : impl_(std::make_unique<Logger::Impl>(level, file, line)) {}

Logger::Logger(const char *file, int line, LogLevel level, const char *func)
    : impl_(std::make_unique<Logger::Impl>(level, file, line)) {
  impl_->stream_ << func << ' ';
}

Logger::~Logger() {
  impl_->finish();
  const LogStream::Buffer &buf(impl_->stream().buffer());
  outputFunc_(buf.data(), buf.length());
  if (impl_->level_ == LogLevel::FATAL) {
    flushFunc_();
    abort();
  }
}

void Logger::setLogLevel(LogLevel level) { level_ = level; }
LogLevel Logger::getLogLevel() { return level_; }

void Logger::setOutput(OutputFunc out) { outputFunc_ = out; }
void Logger::setFlush(FlushFunc flush) { flushFunc_ = flush; }

} // namespace log