#pragma once
#include "command.hpp"

class PingCommand : public Command 
{
   std::string name() const override { return "ping"; }
   std::string description() const override { return "Ping Pong!"; }
   void execute(const dpp::slashcommand_t& event) override;
};