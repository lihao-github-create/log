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

#define LOG_TRACE                                                              \
  if (Logger::getLogLevel() <= LogLevel::TRACE)                                \
  Logger(__FILE__, __LINE__, LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG                                                              \
  if (Logger::getLogLevel() <= LogLevel::DEBUG)                                \
  Logger(__FILE__, __LINE__, LogLevel::DEBUG, __func__).stream()
#define LOG_INFO                                                               \
  if (Logger::getLogLevel() <= LogLevel::INFO)                                 \
  Logger(__FILE__, __LINE__, LogLevel::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, LogLevel::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, LogLevel::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, LogLevel::FATAL).stream()

static LogLevel getLogLevel();
static void setLogLevel(LogLevel level);
static void setOutput(OutputFunc);
static void setFlush(FlushFunc);

} // namespace log