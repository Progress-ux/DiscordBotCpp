#pragma once
#include "command.h"
#include "bot.h"

class BackCommand : public Command 
{
private:
   Bot& bot;
public:
   BackCommand(Bot& b);

   std::string name() const override { return "back"; }
   std::string description() const override { return "Back audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};