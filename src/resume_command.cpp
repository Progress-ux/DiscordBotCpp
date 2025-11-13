#include "resume_command.h"

void ResumeCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   if (!v || !v->voiceclient || !v->voiceclient->is_ready()) 
   {
      event.edit_response("Error: I'm not in the voice channel!");
      return;
   }

   if (!v->voiceclient->is_paused())
   {
      event.reply("Music not paused");
      return;
   }
   
   v->voiceclient->pause_audio(false);
   event.reply("Audio resume");
}