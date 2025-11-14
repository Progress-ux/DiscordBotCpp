#include "back_command.h"

BackCommand::BackCommand(Bot &b) : bot(b) {}

void BackCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if (!v || !v->voiceclient || !v->voiceclient->is_ready()) 
   {
      event.edit_response("Error: I'm not in the voice channel!");
      return;
   }

   if (musicHandler.isHistoryEmpty())
   {
      event.edit_response("History empty!");
      return;
   }

   musicHandler.setBackFlag(true);

   if (!v->voiceclient->is_playing())
      musicHandler.startPlayer(v);
      
   
   event.edit_response("Returning to the previous track!");
}
