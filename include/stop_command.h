#pragma once
#include "command.h"
#include <dpp/dpp.h>

class StopCommand : public Command 
{
   std::string name() const override { return "stop"; }
   std::string description() const override { return "Stop playing audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};