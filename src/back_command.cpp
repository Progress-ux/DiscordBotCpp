#include "back_command.h"

BackCommand::BackCommand(Bot &b) : bot(b) {}

void BackCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if (!musicHandler.voiceclient) 
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
   event.edit_response("Returning to the previous track!");

   if (musicHandler.voiceclient && !musicHandler.voiceclient->is_playing())
      musicHandler.Player();
}
