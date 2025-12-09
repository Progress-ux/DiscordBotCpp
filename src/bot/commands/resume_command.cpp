#include "resume_command.hpp"

void ResumeCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /resume invoked");

   event.thinking(); 
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if (!musicHandler.voiceclient || !musicHandler.voiceclient->is_ready()) 
   {
      event.edit_response("Error: I'm not in the voice channel!");
      return;
   }

   if (!musicHandler.voiceclient->is_paused())
   {
      event.reply("Music not paused");
      return;
   }
   
   musicHandler.voiceclient->pause_audio(false);
   event.reply("Audio resume");
}