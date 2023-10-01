/* =====================================================================================
 *
 *       Filename:  logger.h
 *
 *    Description:  日志输出器
 *
 *        Version:  1.0
 *        Created:
 *       Revision:  none
 *       Compiler:
 *
 *         Author:
 *        Company:
 *
 * =====================================================================================
 */
#include "log_stream.h"
#include <cinttypes>
#include <functional>
#include <memory>

namespace log {
/**
 * 日志级别，从小到大
 */
enum class LogLevel : unsigned long {
  TRACE = 0,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LOG_LEVELS,
};

/**
 * 日志输出器: 输出格式按照如下格式：
 *        日期     时间     微秒    线程   级别   正文    源文件: 行号
 *     20220603  08:30:46  125770  1234  INFO   hello - test.cpp: 12
 */

using OutputFunc = std::function<void(const char *, int)>;
using FlushFunc = std::function<void()>;
class Logger {
public:
  Logger(const char *file, int line, LogLevel level);
  Logger(const char *file, int line, LogLevel level, const char *func);
  ~Logger();

  LogStream &stream();

  static LogLevel getLogLevel();
  static void setLogLevel(LogLevel level);

  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace log