#include "logging.h"

namespace log {
LogLevel getLogLevel() { return Logger::getLogLevel(); }
void setLogLevel(LogLevel level) { Logger::setLogLevel(level); }
void setOutput(OutputFunc func) { Logger::setOutput(func); }
void setFlush(FlushFunc func) { Logger::setFlush(func); }
} // namespace log