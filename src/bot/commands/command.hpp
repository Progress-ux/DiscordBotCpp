#pragma once
#include <string>
#include <dpp/dpp.h>
#include "core/log_macros.hpp"

class Command
{
public:
   virtual ~Command() = default;

   virtual std::string name() const = 0;

   virtual std::string description() const = 0;

   virtual void execute(const dpp::slashcommand_t& event) = 0;
};                   