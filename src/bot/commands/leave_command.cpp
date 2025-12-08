#include "leave_command.hpp"

void LeaveCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /leave invoked");
   
   event.thinking();

   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);
   dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);

   if (!v)
   {
      event.edit_response("You are not in the voice channel!");
      return;
   }
   
   if(musicHandler.voiceclient && musicHandler.voiceclient->is_playing())
   {
      musicHandler.voiceclient->stop_audio();
      musicHandler.setStopFlag(true);
   }

   event.from()->disconnect_voice(v->guild_id);
   event.edit_response("The bot has left the channel!");
}