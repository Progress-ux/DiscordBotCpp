#include "ping_command.h"

void PingCommand::execute(const dpp::slashcommand_t &event)
{
   event.reply("Pong!");
}