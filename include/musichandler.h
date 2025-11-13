#pragma once

#include "track.h"
#include <dpp/dpp.h>
#include <optional>
#include <atomic>
#include <queue>

class MusicHandler
{
private:
   std::queue<Track> queue;
   std::queue<Track> history;

   std::atomic<bool> stop_flag;
   std::atomic<bool> skip_flag;

public:
   void addTrack(Track track);

   void extractInfo(MusicHandler& musichandler, std::string& url);
   void extractInfo(Track &track);

   std::optional<Track> getCurrentTrack() const;
   std::optional<Track> getLastTrack();

   Track getPopNextTrack();
   Track& getNextTrack();

   void setStopFlag(bool s) noexcept { stop_flag.store(s); }
   bool isStopFlag() const noexcept { return stop_flag.load(); }

   void setSkipFlag(bool s) noexcept { skip_flag.store(s); }
   bool isSkipFlag() const noexcept { return skip_flag.load(); }

   void playTrack(std::string stream_url, dpp::voiceconn *v);

   bool isEmpty();

   void updateWorkingStreamLink(Track &track);

   void clear();
   size_t size();
};