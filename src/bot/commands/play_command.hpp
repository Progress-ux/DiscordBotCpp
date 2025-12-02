#pragma once

#include "command.hpp"
#include "bot.hpp"
#include "track.hpp"

class PlayCommand : public Command
{
private:
   Bot& bot;
public:
   PlayCommand(Bot& b);

   std::string name() const override { return "play"; }
   std::string description() const override { return "Play audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};