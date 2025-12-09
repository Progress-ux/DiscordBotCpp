#pragma once
#include "command.hpp"
#include "bot.hpp"

class ResumeCommand : public Command 
{
private:
   Bot &bot;
public:
   ResumeCommand(Bot &b) : bot(b) {}

   std::string name() const override { return "resume"; }
   std::string description() const override { return "Resume audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};