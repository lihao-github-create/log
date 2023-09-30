#include "async_logging.h"
#include "buffer.h"
#include "log_file.h"
#include "mutex_macro.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define GUARDED_BY(x) __attribute__((guarded_by(x)))
constexpr int NUM_BUFFERS = 16;
constexpr int NUM_DROP_BUFFERS_THRESHOLD = 25;

namespace log {

class AsyncLogging::Impl {
public:
  Impl(const string &basename, int rollSize, int flushInterval = 3);

  ~Impl() {
    if (running_) {
      stop();
    }
  }

  void append(const char *logline, size_t len);

  void start() {
    thread_ = std::thread([&]() {
      running_ = true;
      threadFunc();
    });
  }

  void stop() {
    running_ = false;
    cond_.notify_one();
    thread_.join();
  }

private:
  void threadFunc();

  using Buffer = FixedBuffer<kLargeBuffer>;
  using BufferVector = std::vector<std::unique_ptr<Buffer>>;
  using BufferPtr = BufferVector::value_type;

  const int flushInterval_; // 刷新周期
  std::atomic<bool> running_;
  const string basename_;
  const off_t rollSize_; // 日志回滚或归档文件个数
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_ GUARDED_BY(mutex_);
  BufferPtr currentBuffer_ GUARDED_BY(mutex_);
  BufferPtr nextBuffer_ GUARDED_BY(mutex_);
  BufferVector buffers_ GUARDED_BY(mutex_);
};

AsyncLogging::Impl::Impl(const string &basename, int rollSize,
                         int flushInterval)
    : flushInterval_(flushInterval), running_(false), basename_(basename),
      rollSize_(rollSize), currentBuffer_(new Buffer), nextBuffer_(new Buffer) {
  currentBuffer_->bzero();
  nextBuffer_->bzero();
  buffers_.reserve(NUM_BUFFERS);
}

void AsyncLogging::Impl::append(const char *logline, size_t len) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (currentBuffer_->avail() > len) {
    currentBuffer_->append(logline, len);
  } else {
    buffers_.push_back(std::move(currentBuffer_));

    if (nextBuffer_) {
      currentBuffer_ = std::move(nextBuffer_);
    } else {
      currentBuffer_.reset(new Buffer);
    }

    currentBuffer_->append(logline, len);
    cond_.notify_one();
  }
}

void AsyncLogging::Impl::threadFunc() {
  LogFile output(basename_, rollSize_);

  // 后备缓冲区
  BufferPtr newBuffer1(new Buffer);
  BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();

  BufferVector buffersToWrite; // 与buffers_组成双缓冲
  buffersToWrite.reserve(NUM_BUFFERS);

  // 从 buffers_ 读取格式化后的日志
  while (running_) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (buffers_.empty()) {
      cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
    }
    buffers_.push_back(std::move(currentBuffer_));
    currentBuffer_ = std::move(newBuffer1);
    buffersToWrite.swap(buffers_);
    if (!nextBuffer_) {
      nextBuffer_ = std::move(newBuffer2);
    }
    // 若日志输出太快，则丢弃部分日志
    if (buffersToWrite.size() > NUM_DROP_BUFFERS_THRESHOLD) {
      char buf[256] = {0};
      snprintf(buf, sizeof buf, "Drop log message %ld large buffers\n",
               buffersToWrite.size() - 2);
      // 同时输出到终端和日志文件
      fputs(buf, stderr);
      output.append(buf, strlen(buf));
      // 仅保留前两个，用于保证前端的后备缓冲区
      buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
    }
    // 写日志文件
    for (auto &buffer : buffersToWrite) {
      output.append(buffer->data(), buffer->length());
    }

    // 更新newBuffer1、newBuffer2
    if (buffersToWrite.size() > 2) {
      buffersToWrite.resize(2);
    }

    if (!newBuffer1) {
      newBuffer1 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer1.reset();
    }

    if (!newBuffer2) {
      newBuffer2 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer2.reset();
    }

    buffersToWrite.clear();
    output.flush();
  }
  output.flush();
}

AsyncLogging::AsyncLogging(const string &basename, int rollSize,
                           int flushInterval = 3)
    : impl_(std::make_unique<Impl>(basename, rollSize, flushInterval)) {}
AsyncLogging::~AsyncLogging() {}

void AsyncLogging::append(const char *logline, size_t len) {
  impl_->append(logline, len);
}

void AsyncLogging::start() { impl_->start(); }

void AsyncLogging::stop() { impl_->stop(); }

} // namespace log