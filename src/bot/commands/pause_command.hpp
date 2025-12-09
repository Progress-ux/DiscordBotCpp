#pragma once
#include "command.hpp"
#include "bot.hpp"

class PauseCommand : public Command 
{
private:
   Bot &bot;
public:
   PauseCommand(Bot &b) : bot(b) {}

   std::string name() const override { return "pause"; }
   std::string description() const override { return "Paused audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};