#pragma once

#include "command.h"
#include <dpp/dpp.h>
#include "bot.h"

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