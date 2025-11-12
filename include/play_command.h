#pragma once

#include "command.h"
#include "bot.h"
#include "track.h"

class PlayCommand : public Command
{
private:
   Bot& bot;
   bool is_playing;

public:
   PlayCommand(Bot& b);

   std::string name() const override { return "play"; }
   std::string description() const override { return "Play audio"; }
   void execute(const dpp::slashcommand_t& event) override;

   void playTrack(Track track, dpp::voiceconn *v, MusicHandler& musicHandler);
};