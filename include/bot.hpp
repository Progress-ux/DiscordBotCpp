#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <dpp/dpp.h>
#include "command.hpp"
#include "musichandler.hpp"

class Bot
{
private:
   std::unique_ptr<dpp::cluster> bot;
   std::map<std::string, std::string> env_vars;
   std::vector<std::shared_ptr<Command>> commands;

   std::map<dpp::snowflake, std::unique_ptr<MusicHandler>> music_handlers;

   std::map<std::string, std::string> load_env(const std::string& env_file = "../.env");
   std::string getToken();
   void register_events();
   void register_commands();
   
public:
   Bot(const std::string& env_file = "../.env");
   void run();
   void add_command(std::shared_ptr<Command> cmd);

   MusicHandler& getMusicHandler(dpp::snowflake guild_id);
};


