/* =====================================================================================
 *
 *       Filename:  log_file.h
 *
 *    Description:  提供写日志到文件的接口
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

#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include "noncopyable.h"
#include <memory>

namespace log {
class LogFile {
  NOCOPYABLE_DECLARE(LogFile)

public:
  LogFile(const string &basename, int rollSize, int flushInterval = 3);
  ~LogFile();

  void append(const char *logline, size_t len);

  void flush();

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
} // namespace log

#endif
