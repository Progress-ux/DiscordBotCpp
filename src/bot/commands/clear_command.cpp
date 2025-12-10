#include "clear_command.hpp"

void ClearCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /clear invoked");

   event.thinking();
   auto& musicHandler = bot.getMusicHandler(event.command.guild_id);

   dpp::command_interaction cmd_data = event.command.get_command_interaction();

   auto subcommand = cmd_data.options[0];

   if(subcommand.name == "all")
   {
      LOG_DEBUG("SubCommand /all invoked");
      musicHandler.clearAll();
      event.edit_response("Queue and history cleared.");
   } 
   else if(subcommand.name == "history") 
   {
      LOG_DEBUG("SubCommand /history invoked");
      musicHandler.clearHistory();
      event.edit_response("History cleared");
   } 
   else if(subcommand.name == "queue") 
   {
      LOG_DEBUG("SubCommand /queue invoked");
      musicHandler.clearQueue();
      event.edit_response("Queue cleared");
   } 
   else 
   {
      LOG_ERROR("Error: no such command found");
      event.edit_response("Error: no such command found");
   }
}