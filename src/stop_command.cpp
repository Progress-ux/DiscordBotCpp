#include "stop_command.h"
#include <dpp/discordvoiceclient.h>

StopCommand::StopCommand(Bot &b) : bot(b) {}

void StopCommand::execute(const dpp::slashcommand_t &event)
{
   auto& musicHandler = bot.getMusicHandler(event.command.guild_id);
   if(musicHandler.getVoiceClient())
   { 
      musicHandler.setStopFlag(true);
      // musicHandler.getVoiceClient()->stop_audio();
   }
   else
   {
      std::cout << "Недоступен v" << std::endl;
   }
   event.reply("Audio stopped");
}
