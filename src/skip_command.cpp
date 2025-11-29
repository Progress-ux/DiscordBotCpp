#include "skip_command.h"

SkipCommand::SkipCommand(Bot& b) : bot(b) {}

void SkipCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if (!musicHandler.voiceclient) 
   {
      event.edit_response("Error: I'm not in the voice channel!");
      return;
   }

   if(musicHandler.isQueueEmpty())
   {
      event.edit_response("Queue empty!");
      return;
   }
   
   musicHandler.setSkipFlag(true);
   event.edit_response("Audio skiped!");

   if (musicHandler.voiceclient && !musicHandler.voiceclient->is_playing()) 
      musicHandler.Player();

}
