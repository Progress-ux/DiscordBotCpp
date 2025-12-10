#include "bot.hpp"
#include "core/logger.hpp"
#include "core/log_macros.hpp"
#include "ping_command.hpp"
#include "join_command.hpp"
#include "play_command.hpp"
#include "stop_command.hpp"
#include "pause_command.hpp"
#include "resume_command.hpp"
#include "skip_command.hpp"
#include "back_command.hpp"
#include "clear_command.hpp"
#include "leave_command.hpp"
#include "queue_command.hpp"
#include "history_command.hpp"
#include <memory>

int main(int argc, char const *argv[])
{
    std::string path = "bot.log";

    Bot bot;
    Logger::instance().setLevel(LogLevel::Debug);
    Logger::instance().openFile(path);

    LOG_DEBUG("Start adding commands");
    bot.add_command(std::make_shared<PingCommand>());
    bot.add_command(std::make_shared<ClearCommand>(bot));
    
    bot.add_command(std::make_shared<JoinCommand>());
    bot.add_command(std::make_shared<LeaveCommand>(bot));
    
    bot.add_command(std::make_shared<StopCommand>(bot));
    
    bot.add_command(std::make_shared<QueueCommand>(bot));
    bot.add_command(std::make_shared<HistoryCommand>(bot));
    
    bot.add_command(std::make_shared<PauseCommand>(bot));
    bot.add_command(std::make_shared<ResumeCommand>(bot));
    
    bot.add_command(std::make_shared<PlayCommand>(bot));
    
    bot.add_command(std::make_shared<SkipCommand>(bot));
    bot.add_command(std::make_shared<BackCommand>(bot));
    LOG_DEBUG("Commands added");
    
    LOG_INFO("Starting bot");
    bot.run();
    LOG_INFO("Bot stopped");
    Logger::instance().stop();
    return 0;
}
