#pragma once
#include "command.hpp"
#include "bot.hpp"

class QueueCommand : public Command 
{
private:
   Bot& bot;
public:
   QueueCommand(Bot& b) : bot(b) {  }
   
   std::string name() const override { return "queue"; }
   std::string description() const override { return "Shows the current queue"; }
   void execute(const dpp::slashcommand_t& event) override;
};