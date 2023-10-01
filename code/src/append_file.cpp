#include "append_file.h"
#include <assert.h>
#include <iostream>
#include <stdio.h>

namespace log {

constexpr int32_t FILE_BUFS_IZE = 64 * 1024;

class AppendFile::Impl {
public:
  Impl(const string &filename);
  ~Impl();

  void append(const char *logline, size_t len);

  void flush();

  size_t writtenBytes() const;

private:
  FILE *fp_;
  char buffer_[FILE_BUFS_IZE];
  size_t writtenBytes_;
};

AppendFile::Impl::Impl(const string &filename) : writtenBytes_(0) {
  fp_ = fopen(filename.c_str(), "ae");
  assert(fp_ != nullptr);
  setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::Impl::~Impl() {
  flush();
  fclose(fp_);
}

void AppendFile::Impl::append(const char *logline, size_t len) {
  size_t n = fwrite_unlocked(logline, 1, len, fp_);
  if (n != len) {
    fprintf(stderr, "AppendFile::append() failed\n");
  }
  writtenBytes_ += n;
}

void AppendFile::Impl::flush() { fflush(fp_); }

size_t AppendFile::Impl::writtenBytes() const { return writtenBytes_; }

AppendFile::AppendFile(const string &filename)
    : impl_(std::make_unique<Impl>(filename)) {}
AppendFile::~AppendFile() {}

void AppendFile::append(const char *logline, size_t len) {
  impl_->append(logline, len);
}

void AppendFile::flush() { impl_->flush(); }

size_t AppendFile::writtenBytes() const { return impl_->writtenBytes(); }

} // namespace log