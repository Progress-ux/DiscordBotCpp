#include "queue_command.hpp"

void QueueCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking();
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   dpp::embed embed;

   embed.set_title("Очередь треков")
      .set_color(0x5865F2);

   if(musicHandler.isQueueEmpty())
      embed.set_description("Очередь пуста");
   else 
   {
      std::string desc;

      desc += "**Сейчас играет:** " + musicHandler.getCurrentTrack().getTitle() + "\n\n";

      for (size_t i = 0; i < musicHandler.queueSize() && i < 10; i++)
      {
         desc += std::to_string(i+1) + ". " + musicHandler.getQueue()[i].getTitle() + " - " + musicHandler.getQueue()[i].getDuration() + "\n";
      }

      if(musicHandler.queueSize() > 10)
         desc += "В очереди еще " + std::to_string(musicHandler.queueSize() - 10) + " трек/ов";
         
      embed.set_description(desc);
   }

   event.edit_response(embed);
}