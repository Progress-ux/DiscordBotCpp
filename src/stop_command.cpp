#include "stop_command.h"
#include <dpp/discordvoiceclient.h>

StopCommand::StopCommand(Bot &b) : bot(b) {}

void StopCommand::execute(const dpp::slashcommand_t &event)
{
   dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);
   auto& musicHandler = bot.getMusicHandler(event.command.guild_id);
   if(v && v->voiceclient && v->voiceclient->is_ready())
   { 
      musicHandler.setStopFlag(true);
      v->voiceclient->stop_audio();
   }
   event.reply("Audio stopped");
}
