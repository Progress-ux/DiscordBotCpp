#include "bot.h"
#include "ping_command.h"
#include "join_command.h"
#include "play_command.h"
#include "stop_command.h"
#include <memory>

int main(int argc, char const *argv[])
{
    Bot bot;

    bot.add_command(std::make_shared<PingCommand>());
    bot.add_command(std::make_shared<JoinCommand>());
    bot.add_command(std::make_shared<PlayCommand>(bot));
    bot.add_command(std::make_shared<StopCommand>());

    bot.run();
    return 0;
}
