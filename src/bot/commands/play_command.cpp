#include "play_command.hpp"
#include <string>
#include <vector>
#include <memory>

PlayCommand::PlayCommand(Bot &b) : bot(b) {}

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /play invoked");

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

      const auto& val_url = event.get_parameter("url");
      const auto& val_search = event.get_parameter("search");

      const std::string* url_ptr = std::get_if<std::string>(&val_url);
      const std::string* search_ptr = std::get_if<std::string>(&val_search);

      std::string url = (url_ptr != nullptr) ? *url_ptr : "";
      std::string search = (search_ptr != nullptr) ? *search_ptr : "";

      if(!url.empty())
         musicHandler.addTrackByLink(url);
      else if(!search.empty())
         musicHandler.addTrackByQuery(search);
      else
      {
         event.edit_response("Request is empty!");
         return;
      }

      if(musicHandler.isQueueEmpty())
      {
         event.edit_response("Error: Queue empty!");
         return;
      }  
      // replaced response with embed
      Track track = musicHandler.getLastTrack();

      if(track.empty())
      {
         LOG_ERROR("track is empty");
         event.edit_response("Error: track is empty!");
         return;
      }

      dpp::embed embed = dpp::embed()
         .set_color(0x5865F2)
         .set_title(track.getTitle())
         .set_url(track.getUrl())
         .add_field("Author", track.getAuthor(), true)
         .add_field("Duration", track.getDuration(), true)
         .add_field("Added", "<@" + std::to_string(event.command.usr.id) + '>', false);

      if(!track.getThumbnail().empty())
         embed.set_thumbnail(track.getThumbnail());
      
      event.edit_response(embed);

      if (musicHandler.voiceclient && !musicHandler.voiceclient->is_playing())
         musicHandler.startPlayer();
   } 
   catch (const std::exception& e) 
   {
      LOG_ERROR(e.what());
      event.edit_response(std::string("Error: ") + e.what());
   }
}
