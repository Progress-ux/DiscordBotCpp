#include "queue_command.hpp"

void QueueCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking();
   auto &musicHandler = bot.getMusicHandler(event.command.guild_id);

   if(musicHandler.isQueueEmpty())
   {
      event.edit_response("The queue is empty");
      return;
   }

   dpp::embed embed;

   embed.set_title("Track queue")
      .set_color(0x5865F2);

   try 
   {
      for(size_t i = 0; i < musicHandler.queueSize(); i++)
      {
         embed.add_field(std::to_string(i+1) + ". " + musicHandler.getTrackFromQueue(i).getTitle(), 
                         musicHandler.getTrackFromQueue(i).getUrl(), 
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