#include "resume_command.h"

void ResumeCommand::execute(const dpp::slashcommand_t &event)
{
   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   if (!v->voiceclient->is_paused())
   {
      event.reply("Music not paused");
      return;
   }
   
   v->voiceclient->pause_audio(false);
   event.reply("Audio resume");
}