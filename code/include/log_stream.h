/* =====================================================================================
 *
 *       Filename:  log_stream.h
 *
 *    Description:  日志输出流
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

#ifndef __BUFFER_H__

#include "buffer.h"
#include "noncopyable.h"
#include "string_piece.h"
#include <cstddef>
#include <cstring>
#include <string>

using std::string;

namespace log {

/**
 * log stream: 内部采用固定缓冲区，大小为 kSmallBuffer = 4000 bytes
 *            注意不要输出过多字节到 LogStream对象中
 * */
class LogStream {
  NOCOPYABLE_DECLARE(LogStream)
  using self = LogStream;

public:
  using Buffer = FixedBuffer<kSmallBuffer>;

  LogStream() = default;

  self &operator<<(bool v) {
    buffer_.append(v ? "1" : "0", 1);
    return *this;
  }

  self &operator<<(short);
  self &operator<<(unsigned short);
  self &operator<<(int);
  self &operator<<(unsigned int);
  self &operator<<(long);
  self &operator<<(unsigned long);
  self &operator<<(long long);
  self &operator<<(unsigned long long);

  self &operator<<(const void *);

  self &operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
  }
  self &operator<<(double);
  // self& operator<<(long double);

  self &operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
  }

  // self& operator<<(signed char);
  // self& operator<<(unsigned char);

  self &operator<<(const char *str) {
    if (str) {
      buffer_.append(str, strlen(str));
    } else {
      buffer_.append("(null)", 6);
    }
    return *this;
  }

  self &operator<<(const unsigned char *str) {
    return operator<<(reinterpret_cast<const char *>(str));
  }

  self &operator<<(const string &v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
  }

  self &operator<<(const StringPiece &v) {
    buffer_.append(v.data(), v.size());
    return *this;
  }

  self &operator<<(const Buffer &v) {
    *this << v.toStringPiece();
    return *this;
  }

  void append(const char *data, int len) { buffer_.append(data, len); }
  const Buffer &buffer() const { return buffer_; }
  void resetBuffer() { buffer_.reset(); }

private:
  void staticCheck();

  template <typename T> void formatInteger(T);

  Buffer buffer_;

  static const int kMaxNumericSize = 48;
};
} // namespace log
#endif