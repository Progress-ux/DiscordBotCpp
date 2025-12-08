#include "logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger &Logger::instance()
{
   static Logger logger;
   return logger;
}

Logger::Logger()
{
   worker = std::thread(&Logger::workerLoop, this);
}

std::string Logger::levelToString(LogLevel level)
{

   switch (level)
   {
      case LogLevel::Debug: return "DEBUG";
      case LogLevel::Info: return "INFO";
      case LogLevel::Warn: return "WARN";
      case LogLevel::Error: return "ERROR";
   }
   return "UNKNOWN";
}

std::string Logger::currentTime()
{
   auto now = std::chrono::system_clock::now();
   auto t = std::chrono::system_clock::to_time_t(now);

   std::tm tm{};
#ifdef _WIN32
   localtime_s(&tm, &t);
#else
   localtime_r(&t, &tm);
#endif

   std::ostringstream ss;
   ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
   return ss.str();
}

Logger::~Logger()
{
   stop();
}

void Logger::setLevel(LogLevel level)
{
   current_level = level;
}

void Logger::openFile(std::string &path)
{
   file.open(path, std::ios::app);
}

void Logger::log(LogLevel level, 
   const std::string &message, 
   const char *file, 
   int line, const 
   char *func)
{
   if(level < current_level) return;

   std::ostringstream ss;
   ss << currentTime() 
   << " [" << levelToString(level) << "] " 
   << file << ":" << line 
   << " " << func 
   << " | " << message;

   {
      std::lock_guard<std::mutex> lock(mutex);
      queue.push({ level, ss.str() });
   }
   cv.notify_one();
}

void Logger::workerLoop()
{
   while(running || !queue.empty())
   {
      std::unique_lock<std::mutex> lock(mutex);
      cv.wait(lock, [&] {
         return !queue.empty() || !running;
      });

      while(!queue.empty())
      {
         auto msg = queue.front();
         queue.pop();
         lock.unlock();

         std::cout << msg.message << "\n";
         if(file.is_open())
         {
            file << msg.message << "\n";
            file.flush();
         }
         lock.lock();
      }
   }
}

void Logger::stop()
{
   running = false;
   cv.notify_all();
   if(worker.joinable())
      worker.join();
}