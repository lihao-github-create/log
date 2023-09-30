/* =====================================================================================
 *
 *       Filename:  current_thread.h
 *
 *    Description:  维护当前线程的状态信息
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

#include <thread>
namespace log {
namespace currentthread {
// internal
extern thread_local int t_cachedTid;
extern thread_local char t_tidString[32];
extern thread_local int t_tidStringLength;
extern thread_local const char *t_threadName;

void cacheTid();

inline int tid() {
  if (__builtin_expect(t_cachedTid == 0, 0)) {
    cacheTid();
  }
  return t_cachedTid;
}

// 以' '+ '\0'作为结尾
inline const char *tidString() // for logging
{
  return t_tidString;
}

inline int tidStringLength() // for logging
{
  return t_tidStringLength;
}

inline const char *name() { return t_threadName; }

bool isMainThread();

} // namespace currentthread
} // namespace log