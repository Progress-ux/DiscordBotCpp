#include "play_command.h"
#include <string>
#include <vector>

PlayCommand::PlayCommand(Bot &b) : bot(b) {}

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   try {
      dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);
      
      // Плеер по guild_id сервера
      // Хранит историю треков
      auto& musicHandler = bot.getMusicHandler(event.command.guild_id);

      if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
         event.edit_response("Error: I'm not in the voice channel!");
         return;
      }
      
      auto val = event.get_parameter("url");
      std::string val_url = std::get<std::string>(val);
      
      musicHandler.extractInfo(musicHandler, val_url);

      event.edit_response("**" + musicHandler.getCurrentTrack()->getTitle() + "**" + " added to queue");
      
      while(!musicHandler.isEmpty())
      {
         if(v->voiceclient->is_playing())
            continue;
            
         musicHandler.playTrack(musicHandler.getNextTrack().getStreamUrl(), v);
      }
   } 
   catch (const std::exception& e) 
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
}
