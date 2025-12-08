#include "back_command.hpp"

BackCommand::BackCommand(Bot &b) : bot(b) {}

void BackCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /back invoked");
   
   event.thinking(); 
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if(!musicHandler.voiceclient)
   {
      dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);
      if(v && v->voiceclient)
      {
         std::shared_ptr<dpp::discord_voice_client> vc_shared = std::shared_ptr<dpp::discord_voice_client>(std::move(v->voiceclient));
         musicHandler.setVoiceClient(vc_shared);
      }
   }

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

   musicHandler.voiceclient->stop_audio();
   musicHandler.setBackFlag(true);

   event.edit_response("Returning to the previous track!");

   if (musicHandler.voiceclient && !musicHandler.voiceclient->is_playing())
      musicHandler.startPlayer();

}
