#include "async_logging.h"
#include "logging.h"

#include <chrono>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace log;
using namespace std;

off_t kRollSize = 5 * 1000;

AsyncLogging *g_asyncLog = NULL;
// 设置异步，还需指定异步输出函数，非常不友好
void asyncOutput(const char *msg, int len) { g_asyncLog->append(msg, len); }

void bench(bool longLog) {
  setOutput(asyncOutput);

  int cnt = 0;
  const int kBatch = 1000000;
  string empty = " ";
  string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 10; ++t) {
    clock_t start = clock();

    for (int i = 0; i < kBatch; ++i) {
      LOG_INFO << "Hello 0123456789"
               << " abcdefghijklmnopqrstuvwxyz " << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    // 获取结束时间点
    clock_t end = clock();
    // 计算处理器时间差
    double cpu_time_used = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    // 输出执行时间
    std::cout << "函数执行时间: " << cpu_time_used << " 秒" << std::endl;
  }
}

int main(int argc, char *argv[]) {
  {
    // set max virtual memory to 2GB.
    size_t kOneGB = 1000 * 1024 * 1024;
    rlimit rl = {2 * kOneGB, 2 * kOneGB};
    setrlimit(RLIMIT_AS, &rl);
  }

  printf("pid = %d\n", getpid());

  char name[256] = {'\0'};
  strncpy(name, argv[0], sizeof name - 1);
  AsyncLogging log(::basename(name), kRollSize);
  log.start();
  g_asyncLog = &log;

  bool longLog = argc > 1;
  bench(longLog);

  std::cout << "Done" << std::endl;
}
