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

      musicHandler.addTrack(val_url);

      if(musicHandler.isQueueEmpty())
      {
         event.edit_response("Error: Queue empty!");
         return;
      }  

      Track track = musicHandler.getLastTrack();

      std::string duration = musicHandler.formatDuration(track.getDuration());

      dpp::embed embed = dpp::embed()
         .set_color(0x5865F2)
         .set_title(track.getTitle())
         .set_url(track.getUrl())
         .add_field("Author", track.getAuthor(), true)
         .add_field("Duration", duration, true)
         .add_field("Added", "<@" + std::to_string(event.command.usr.id) + '>', false);

      if(!track.getThumbnail().empty())
         embed.set_thumbnail(track.getThumbnail());
      
      event.edit_response(embed);

      if (!musicHandler.voiceclient || musicHandler.voiceclient->is_playing())
         return;

      musicHandler.Player();
   } 
   catch (const std::exception& e) 
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
}
