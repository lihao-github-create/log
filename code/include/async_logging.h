/* =====================================================================================
 *
 *       Filename:  async_logging.h
 *
 *    Description:  异步日志输出器
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

#ifndef __ASYNC_LOGGING_H__
#define __ASYNC_LOGGING_H__

#include "noncopyable.h"
#include <atomic>
#include <memory>
#include <mutex>

namespace log {
class AsyncLogging {
  NOCOPYABLE_DECLARE(AsyncLogging);

public:
  AsyncLogging(const std::string &basename, int rollSize,
               int flushInterval = 3);
  ~AsyncLogging();

  void append(const char *logline, size_t len);

  void start();

  void stop();

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
} // namespace log

#endif
