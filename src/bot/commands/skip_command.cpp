#include "skip_command.hpp"

SkipCommand::SkipCommand(Bot& b) : bot(b) {}

void SkipCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /skip invoked");

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

   if(musicHandler.isQueueEmpty())
   {
      event.edit_response("Queue empty!");
      return;
   }
   
   musicHandler.setSkipFlag(true);
   musicHandler.voiceclient->stop_audio();
   event.edit_response("Audio skiped!");

   if (musicHandler.voiceclient && !musicHandler.voiceclient->is_playing()) 
      musicHandler.startPlayer();

}
