#pragma once
#include <string>
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

enum class LogLevel
{
   Debug,
   Info,
   Warn,
   Error
};

struct LogMessage
{
   LogLevel level;
   std::string message;
};

class Logger 
{
private:
   Logger();
   Logger(const Logger&) = delete;
   ~Logger();
   Logger& operator=(const Logger&) = delete;

   void workerLoop();

   LogLevel current_level = LogLevel::Debug;
   std::ofstream file;

   std::queue<LogMessage> queue;
   std::mutex mutex;
   std::condition_variable cv;
   std::thread worker;
   std::atomic<bool> running{true};

   std::string levelToString(LogLevel level);
   std::string currentTime();
public:
   static Logger& instance();

   void setLevel(LogLevel level);
   void openFile(std::string &path);

   void log(LogLevel level,
            const std::string &message,
            const char *file,
            int line,
            const char *func);
   void stop();
};