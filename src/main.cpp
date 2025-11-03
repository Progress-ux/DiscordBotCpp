#include "bot.h"
#include "ping_command.h"
#include <memory>

int main(int argc, char const *argv[])
{
    Bot bot;

    bot.add_command(std::make_shared<PingCommand>());

    bot.run();
    return 0;
}
