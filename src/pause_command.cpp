#include "pause_command.h"

void PauseCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   if (!v || !v->voiceclient || !v->voiceclient->is_ready()) 
   {
      event.edit_response("Error: I'm not in the voice channel!");
      return;
   }

   if (!v->voiceclient->is_playing())
   {
      event.reply("Music not playing");
      return;
   }
   
   v->voiceclient->pause_audio(true);
   event.reply("Audio paused");
}