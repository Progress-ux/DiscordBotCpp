#include "play_command.h"
#include <string>
#include <vector>
#include <memory>

PlayCommand::PlayCommand(Bot &b) : bot(b) {}

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 
   
   try {
      dpp::voiceconn *v = event.from()->get_voice(event.command.guild_id);
      /**
       * @note Музыкальный проигрыватель
       */
      auto& musicHandler = bot.getMusicHandler(event.command.guild_id);
      if (!v) 
      {
         event.edit_response("Error: I'm not in the voice channel!");
         return;
      }

      auto val = event.get_parameter("url");
      std::string val_url = std::get<std::string>(val);
      
      if(!musicHandler.isValidUrl(val_url))
      {
         event.edit_response("Error: Invalid link entered!");
         return;
      }

      Track track = musicHandler.extractInfo(val_url);
      
      if(track.empty())
      {
         event.edit_response("Error: Failed to retrieve information!");
         return;
      }
      
      musicHandler.addTrack(track);
      
      std::string duration = track.getDuration(); // in seconds
      int minutes = std::stoi(duration) / 60;
      int seconds = std::stoi(duration) % 60;
      
      std::string response = 
      "**Title:** " + track.getTitle() + "\n" +
      "**Artist:** " + track.getAuthor() + "\n" +
      "**Duration:** " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds) + "\n" +
      "**Queue Position:** " + std::to_string(musicHandler.queueSize());
      
      event.edit_response(response);
      
      if (!v /* && !isPlaying*/) //TODO: Add play flag
         return;

      musicHandler.Player(v);
   } 
   catch (const std::exception& e) 
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
}
