#include "bot.h"
#include "ping_command.h"
#include "join_command.h"
#include "play_command.h"
#include "stop_command.h"
#include "pause_command.h"
#include "resume_command.h"
#include "skip_command.h"
#include "back_command.h"
#include "leave_command.h"
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
