#pragma once
#include "command.h"
#include "bot.h"

class SkipCommand : public Command 
{
private:
   Bot& bot;
public:
   SkipCommand(Bot& b);

   std::string name() const override { return "skip"; }
   std::string description() const override { return "Skip audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};