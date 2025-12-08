#include "history_command.hpp"

void HistoryCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /history invoked");

   event.thinking();
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if(musicHandler.isHistoryEmpty())
   {
      event.edit_response("The story is empty");
      return;
   }

   dpp::embed embed;

   embed.set_title("Track history")
      .set_color(0x5865F2);


   try 
   {
      for(size_t i = 0; i < musicHandler.historySize(); i++)
      {
         embed.add_field(std::to_string(i+1) + ". " + musicHandler.getTrackFromHistory(i).getTitle(), 
                         musicHandler.getTrackFromHistory(i).getUrl(), 
                         false
         );
         if(i >= 5)
            break;
      }
      event.edit_response(embed);
   }
   catch (std::exception& e)
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
   
}