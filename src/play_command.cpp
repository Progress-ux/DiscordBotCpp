#include "play_command.h"
#include <string>
#include <vector>
#include <memory>

PlayCommand::PlayCommand(Bot &b) : bot(b) {}

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 
   
   try 
   {
      auto& musicHandler = bot.getMusicHandler(event.command.guild_id);
      
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
      
      auto val = event.get_parameter("url");
      std::string val_url = std::get<std::string>(val);

      if(!musicHandler.isValidUrl(val_url))
      {
         event.edit_response("Error: Incorrect link!");
         return;
      }

      std::string response = musicHandler.addTrack(val_url);
      
      event.edit_response(response);

      

      if (!musicHandler.voiceclient || musicHandler.voiceclient->is_playing())
         return;

      musicHandler.Player();
   } 
   catch (const std::exception& e) 
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
}
