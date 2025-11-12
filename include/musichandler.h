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

   int64_t current_pts = 0;

   std::atomic<bool> stop_flag;

public:
   void addTrack(Track track);

   void extractInfo(MusicHandler& musichandler, std::string& url);

   std::optional<Track> getCurrentTrack() const;
   Track getNextTrack();

   void setStopFlag(bool s) noexcept { stop_flag.store(s); }
   bool isStopFlag() const noexcept { return stop_flag.load(); }

   void playTrack(std::string url, dpp::voiceconn *v);

   bool isEmpty();

   void clear();
   size_t size();
};