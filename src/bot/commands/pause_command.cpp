#include "pause_command.hpp"

void PauseCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /pause invoked");

   event.thinking(); 
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   if (!musicHandler.voiceclient || !musicHandler.voiceclient->is_ready()) 
   {
      event.edit_response("Error: I'm not in the voice channel!");
      return;
   }

   if (!musicHandler.voiceclient->is_playing())
   {
      event.edit_response("Music not playing");
      return;
   }
   
   musicHandler.voiceclient->pause_audio(true);
   event.edit_response("Audio paused");
}