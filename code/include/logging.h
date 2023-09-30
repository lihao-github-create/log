/* =====================================================================================
 *
 *       Filename:  logging.h
 *
 *    Description:  包含日志库需要暴露给用户的接口
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

#include "logger.h"

namespace log {

extern LogLevel getLogLevel();
extern void setLogLevel(LogLevel level);
extern void setOutput(OutputFunc);
extern void setFlush(FlushFunc);

#define LOG_TRACE                                                              \
  if (getLogLevel() <= LogLevel::TRACE)                                        \
  Logger(__FILE__, __LINE__, LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG                                                              \
  if (getLogLevel() <= LogLevel::DEBUG)                                        \
  Logger(__FILE__, __LINE__, LogLevel::DEBUG, __func__).stream()
#define LOG_INFO                                                               \
  if (getLogLevel() <= LogLevel::INFO)                                         \
  Logger(__FILE__, __LINE__, LogLevel::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, LogLevel::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, LogLevel::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, LogLevel::FATAL).stream()

} // namespace log