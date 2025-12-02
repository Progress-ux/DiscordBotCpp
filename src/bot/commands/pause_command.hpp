#pragma once
#include "command.hpp"

class PauseCommand : public Command 
{
   std::string name() const override { return "pause"; }
   std::string description() const override { return "Paused audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};