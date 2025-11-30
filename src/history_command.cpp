#include "history_command.hpp"

void HistoryCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking();
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   dpp::embed embed;

   embed.set_title("История треков")
      .set_color(0x5865F2);

   if(musicHandler.isHistoryEmpty())
      embed.set_description("История пуста");
   else 
   {
      std::string desc;

      size_t count = musicHandler.historySize();
      size_t start = (count > 10 ? count - 10 : 0);

      for (size_t i = count; i-- > start;)
      {
         desc += std::to_string(i) + ". " + musicHandler.getHistory()[i].getTitle() + " - " + musicHandler.getHistory()[i].getDuration() + "\n";
      }

      if(count > 10)
         desc += "В истории еще " + std::to_string(count - 10) + " трек/ов";
      embed.set_description(desc);
   }
   
   event.edit_response(embed);
}