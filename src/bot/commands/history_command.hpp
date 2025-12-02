#pragma once
#include "command.hpp"
#include "bot.hpp"

class HistoryCommand : public Command 
{
private:
   Bot& bot;
public:
   HistoryCommand(Bot& b) : bot(b) {  }
   
   std::string name() const override { return "history"; }
   std::string description() const override { return "Shows current history"; }
   void execute(const dpp::slashcommand_t& event) override;
};