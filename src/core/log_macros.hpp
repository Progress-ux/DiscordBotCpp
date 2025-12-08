#define LOG_DEBUG(msg) Logger::instance().log(LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define LOG_INFO(msg)  Logger::instance().log(LogLevel::Info,  msg, __FILE__, __LINE__, __func__)
#define LOG_WARN(msg)  Logger::instance().log(LogLevel::Warn,  msg, __FILE__, __LINE__, __func__)
#define LOG_ERROR(msg) Logger::instance().log(LogLevel::Error, msg, __FILE__, __LINE__, __func__)
