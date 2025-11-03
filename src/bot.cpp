#include "bot.h"
#include <fstream>

Bot::Bot(const std::string& env_file) {
   env_vars = load_env(env_file);

   std::string token = getToken();
   if(token.empty()) {
      std::cerr << "Error: TOKEN not found or empty!" << std::endl;
      std::exit(EXIT_FAILURE);
   }

   bot = std::make_unique<dpp::cluster>(token);
   
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

std::string Bot::getToken()
{
   auto it = env_vars.find("TOKEN");
   if(it != env_vars.end()) return it->second;
   std::cerr << "Error: Token not found in .env" << std::endl;
   return ""; 
}

void Bot::register_events()
{
   bot->on_log(dpp::utility::cout_logger());

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
         register_commands();
      }
   });
}

void Bot::register_commands()
{
   for(auto& cmd : commands)
   {
      bot->global_command_create(dpp::slashcommand(cmd->name(), cmd->description(), bot->me.id));
   }
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
      else
      {
         std::cerr << "Error: Could not open .env file: " << filename << std::endl;
      }
      return vars;
}

