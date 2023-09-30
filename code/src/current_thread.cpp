#include "current_thread.h"

#include <cstdio>
#include <unistd.h>

namespace log {

namespace currentthread {

thread_local int t_cachedTid = 0;
thread_local char t_tidString[32] = {'\0'};
thread_local int t_tidStringLength = 6;
thread_local const char *t_threadName = "unknown";

void cacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = pthread_self();
    t_tidStringLength =
        snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

bool isMainThread() { return tid() == getpid(); }

} // namespace currentthread

} // namespace log