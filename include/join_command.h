#pragma once
#include "command.h"
#include <dpp/dpp.h>

class JoinCommand : public Command 
{
   std::string name() const override { return "join"; }
   std::string description() const override { return "Join your voice channel"; }
   void execute(const dpp::slashcommand_t& event) override;
};