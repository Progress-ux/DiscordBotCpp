#include "bot.hpp"
#include "ping_command.hpp"
#include "join_command.hpp"
#include "play_command.hpp"
#include "stop_command.hpp"
#include "pause_command.hpp"
#include "resume_command.hpp"
#include "skip_command.hpp"
#include "back_command.hpp"
#include "leave_command.hpp"
#include <memory>

int main(int argc, char const *argv[])
{
    Bot bot;

    bot.add_command(std::make_shared<PingCommand>());
    
    bot.add_command(std::make_shared<JoinCommand>());
    bot.add_command(std::make_shared<LeaveCommand>(bot));
    
    bot.add_command(std::make_shared<StopCommand>(bot));
    
    bot.add_command(std::make_shared<PauseCommand>());
    bot.add_command(std::make_shared<ResumeCommand>());

    bot.add_command(std::make_shared<PlayCommand>(bot));
    
    bot.add_command(std::make_shared<SkipCommand>(bot));
    bot.add_command(std::make_shared<BackCommand>(bot));

    bot.run();
    return 0;
}
