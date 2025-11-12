#pragma once
#include "command.h"
#include "bot.h"

class StopCommand : public Command 
{
   std::string name() const override { return "stop"; }
   std::string description() const override { return "Stops audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};