#pragma once
#include "command.h"

class JoinCommand : public Command 
{
   std::string name() const override { return "join"; }
   std::string description() const override { return "The bot will join your voice channel."; }
   void execute(const dpp::slashcommand_t& event) override;
};