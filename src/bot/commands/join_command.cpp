#include "join_command.hpp"

void JoinCommand::execute(const dpp::slashcommand_t& event)
{
   LOG_DEBUG("Command /join invoked");

   dpp::guild* g = dpp::find_guild(event.command.guild_id);
   if(!g)
   {
      event.reply("Guild not found");
      return;
   }

   if (!g->connect_member_voice(*event.owner, event.command.get_issuing_user().id)) {
      event.reply("You don't seem to be in a voice channel!");
      return;
   }
   event.reply("Joined your channel!");
}