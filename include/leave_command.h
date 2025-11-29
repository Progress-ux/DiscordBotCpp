#pragma once
#include "command.h"
#include "bot.h"

class LeaveCommand : public Command 
{
private:
   Bot& bot;
public:
   LeaveCommand(Bot& b) : bot(b) {  }
   
   std::string name() const override { return "leave"; }
   std::string description() const override { return "Coming out of the channel"; }
   void execute(const dpp::slashcommand_t& event) override;
};