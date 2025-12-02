#include "ping_command.hpp"

void PingCommand::execute(const dpp::slashcommand_t &event)
{
   event.reply("Pong!");
}