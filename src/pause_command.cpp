#include "pause_command.h"

void PauseCommand::execute(const dpp::slashcommand_t &event)
{
   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   if (!v->voiceclient->is_playing())
   {
      event.reply("Music not playing");
      return;
   }
   
   v->voiceclient->pause_audio(true);
   event.reply("Audio paused");
}