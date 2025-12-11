#pragma once
#include "command.hpp"
#include "bot.hpp"

class RepeatCommand : public Command 
{
private:
   Bot& bot;
public:
   RepeatCommand(Bot& b) : bot(b) {  }
   
   std::string name() const override { return "repeat"; }
   std::string description() const override { return "Enables/disables track auto-repeat"; }
   void execute(const dpp::slashcommand_t& event) override;
};