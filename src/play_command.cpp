#include "play_command.h"

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   event.reply("Play Command correct");
}