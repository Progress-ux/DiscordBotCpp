#include "bot.hpp"
#include <fstream>
#include "core/log_macros.hpp"
#include <dpp/intents.h>

Bot::Bot(const std::string& env_file) {
   env_vars = load_env(env_file);

   std::string token = getToken();
   if(token.empty()) {
      LOG_ERROR("Error: TOKEN not found or empty!");
      std::exit(EXIT_FAILURE);
   }

   bot = std::make_unique<dpp::cluster>(token, dpp::i_default_intents | dpp::i_guild_voice_states | dpp::i_guilds);
   
   register_events();
}

void Bot::run()
{
   bot->start(dpp::st_wait);
}

void Bot::add_command(std::shared_ptr<Command> cmd)
{
   commands.push_back(cmd);
}

MusicHandler &Bot::getMusicHandler(dpp::snowflake guild_id)
{
   if (music_handlers.find(guild_id) == music_handlers.end()) {
      music_handlers[guild_id] = std::make_unique<MusicHandler>(guild_id);
      LOG_DEBUG("Created MusicHandler for server: " + guild_id);
   }

   return *music_handlers[guild_id];
}

std::string Bot::getToken()
{
   auto it = env_vars.find("TOKEN");
   if(it != env_vars.end()) return it->second;
   return ""; 
}

void Bot::register_events()
{
   bot->on_slashcommand([this](const dpp::slashcommand_t& event) {
      for(auto& cmd : commands) {
         if(event.command.get_command_name() == cmd->name()) {
            cmd->execute(event);
            return;
         }
      }
   });
   bot->on_ready([this](const dpp::ready_t& event) {
      if(dpp::run_once<struct register_bot_commands>()) {
         LOG_INFO("Bot Ready");
         register_commands();
      }
   });
}

void Bot::register_commands()
{
   std::vector<dpp::slashcommand> cmds;
   for(auto& cmd : commands)
   {
      dpp::slashcommand slash (cmd->name(), cmd->description(), bot->me.id);

      if(cmd->name() == "play")
      {
         slash.add_option(
            dpp::command_option(dpp::co_string, "url", "Link to youtube video", true)
         );       
      }
      cmds.push_back(slash);
   }
   
   bot->global_bulk_command_create(cmds);
}

std::map<std::string, std::string> Bot::load_env(const std::string& filename)
{
   std::map<std::string, std::string> vars;
   std::ifstream file(filename);
      if(file.is_open())
      {
         std::string line;
         while (getline(file, line))
         {
               if (line.empty() || line[0] == '#') continue;
               size_t pos = line.find('=');
               if (pos != std::string::npos)
               {
                  vars[line.substr(0,pos)] = line.substr(pos+1);
               }
         }
         file.close();
      }
      return vars;
}

