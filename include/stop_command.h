#pragma once
#include "command.h"
#include "bot.h"

class StopCommand : public Command 
{
private:
   Bot& bot;
public:
   StopCommand(Bot& b);
   std::string name() const override { return "stop"; }
   std::string description() const override { return "Stops audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};