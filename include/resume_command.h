#pragma once
#include "command.h"

class ResumeCommand : public Command 
{
   std::string name() const override { return "resume"; }
   std::string description() const override { return "Resume audio"; }
   void execute(const dpp::slashcommand_t& event) override;
};