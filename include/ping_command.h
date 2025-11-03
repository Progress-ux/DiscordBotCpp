#pragma once
#include "command.h"
#include <dpp/dpp.h>

class PingCommand : public Command 
{
   std::string name() const override { return "ping"; }
   std::string description() const override { return "Ping Pong!"; }
   void execute(const dpp::slashcommand_t& event) override;
};