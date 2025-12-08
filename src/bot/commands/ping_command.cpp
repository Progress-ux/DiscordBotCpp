#include "ping_command.hpp"

void PingCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /ping invoked");
   event.reply("Pong!");
}