#include "repeat_command.hpp"

void RepeatCommand::execute(const dpp::slashcommand_t &event)
{
   LOG_DEBUG("Command /repeat invoked");

   event.thinking();
   auto& musicHandler = bot.getMusicHandler(event.command.guild_id);

   musicHandler.setRepeatFlag(!musicHandler.isRepeatFlag());
   std::string status = musicHandler.isRepeatFlag() ? "enabled" : "disabled";
   event.edit_response("Repeat is: " + status);
}