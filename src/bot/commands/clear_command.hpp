#pragma once
#include "command.hpp"
#include "bot.hpp"

class ClearCommand : public Command 
{
private:
   Bot& bot;
public:
   ClearCommand(Bot& b) : bot(b) {  }
   
   std::string name() const override { return "clear"; }
   std::string description() const override { return "Clears history and/or queue"; }
   void execute(const dpp::slashcommand_t& event) override;
};