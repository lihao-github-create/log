/* =====================================================================================
 *
 *       Filename:  append_file.h
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

#ifndef __APPEND_FILE_H__
#define __APPEND_FILE_H__

#include "noncopyable.h"
#include <memory>
#include <string>
using std::string;

namespace log {
class AppendFile {
  NOCOPYABLE_DECLARE(AppendFile)

public:
  AppendFile(const string &filename);
  ~AppendFile();

  void append(const char *logline, size_t len);

  void flush();

  size_t writtenBytes() const;

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
} // namespace log

#endif