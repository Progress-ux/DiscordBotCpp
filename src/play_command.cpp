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

      if (!v || !v->voiceclient || !v->voiceclient->is_ready()) 
      {
         event.edit_response("Error: I'm not in the voice channel!");
         return;
      }
      
      auto val = event.get_parameter("url");
      std::string val_url = std::get<std::string>(val);
      
      musicHandler.extractInfo(val_url);

      auto &lastTrack = musicHandler.getLastTrack();

      std::string duration = lastTrack.getDuration(); // in seconds
      int minutes = std::stoi(duration) / 60;
      int seconds = std::stoi(duration) % 60;

      std::string response = 
         "**Title:** " + lastTrack.getTitle() + "\n" +
         "**Artist:** " + lastTrack.getAuthor() + "\n" +
         "**Duration:** " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds) + "\n" +
         "**Queue Position:** " + std::to_string(musicHandler.size());

      event.edit_response(response);

      if(v->voiceclient->is_playing())
         return;

      musicHandler.startPlayer(v);
   } 
   catch (const std::exception& e) 
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
}
