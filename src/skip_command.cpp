#include "skip_command.h"

SkipCommand::SkipCommand(Bot& b) : bot(b) {}

void SkipCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if (!v) 
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

   if (v /* && !isPlaying*/) //TODO: Add play flag
      musicHandler.Player();

}
