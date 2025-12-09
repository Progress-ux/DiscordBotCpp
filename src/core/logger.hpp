#pragma once
#include <string>
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

/**
 * @brief Logging levels
 * 
 */
enum class LogLevel
{
   Debug,
   Info,
   Warn,
   Error
};

/**
 * @brief Structure for storing message queue
 * 
 */
struct LogMessage
{
   LogLevel level;
   std::string message;
};
/**
 * @brief Asynchronous logger
 * Logging is performed in a separate thread, writes logs to the bot.log file
 */
class Logger 
{
private:
   /**
    * @brief Construct a new Logger object.
    *  Starts a worker thread
    */
   Logger();
   Logger(const Logger&) = delete;

   /**
    * @brief Destroy the Logger object. Calls stop() to terminate the thread
    * 
    */
   ~Logger();

   Logger& operator=(const Logger&) = delete;

   /**
    * @brief The main work cycle. 
    * 
    * Waits for messages and outputs them to the console/file
    */
   void workerLoop();

   LogLevel current_level = LogLevel::Debug; // Current minimum level
   std::ofstream file;                       // Output stream to file

   std::queue<LogMessage> queue;             // Message Queue
   std::mutex mutex;                         // To synchronize queue access
   std::condition_variable cv;               // Condition variable to notify the worker thread of new messages.
   std::thread worker;                       // Stream for running the logger
   std::atomic<bool> running{true};          // A flag indicating that the logger is running (used to terminate the stream).

   /**
    * @brief Converts the log level to a string representation (for example, "DEBUG").
    */
   std::string levelToString(LogLevel level);

   /**
    * @brief Returns the current time in a formatted string.
    */
   std::string currentTime();

   /**
    * @brief Returns the ANSI color code for the specified log level (for the console).
    */
   std::string levelColor(LogLevel level);
public:

   /**
    * @brief Static access method to a single logger instance.
    * @return Link to a single instance of Logger.
    */
   static Logger& instance();

   /**
    * @brief Sets the minimum logging level. Messages below this level are ignored.
    * @param level New minimum level.
    */
   void setLevel(LogLevel level);

   /**
    * @brief Opens a file for logging (in append mode).
    * @param path Path to the log file.
    */
   void openFile(std::string &path);

   /**
   * @brief Adds a message to the logging queue.
   * 
   * This method is exposed through wrapper macros.
   * 
   * @param level Message level.
   * @param message Message text.
   * @param file Source file name.
   * @param line Line number.
   * @param func Function name.
   */
   void log(LogLevel level,
            const std::string &message,
            const char *file,
            int line,
            const char *func);
   void stop();
};