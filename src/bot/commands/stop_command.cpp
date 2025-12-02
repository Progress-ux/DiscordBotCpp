#include "stop_command.hpp"
#include <dpp/discordvoiceclient.h>

StopCommand::StopCommand(Bot &b) : bot(b) {}

void StopCommand::execute(const dpp::slashcommand_t &event)
{
   auto& musicHandler = bot.getMusicHandler(event.command.guild_id);
   if(musicHandler.voiceclient)
   { 
      musicHandler.setStopFlag(true);
      musicHandler.voiceclient->stop_audio();
      event.reply("Audio stopped");
   }
   else
      event.reply("Nothing plays!");
}
