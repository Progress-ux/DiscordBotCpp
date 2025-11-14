#pragma once

#include "track.h"
#include <dpp/dpp.h>
#include <atomic>
#include <vector>

class MusicHandler
{
private:
   size_t current_index = 0;
   std::vector<Track> playlist;

   std::atomic<bool> stop_flag;
   std::atomic<bool> skip_flag;
   std::atomic<bool> back_flag;

   void playTrack(std::string stream_url, dpp::voiceconn *v);
public:
   void addTrack(Track track);

   void extractInfo(std::string& url);
   void extractInfo(Track &track);

   Track& getCurrentTrack();
   Track& getLastTrack();

   Track& getNextTrack();
   Track& getBackTrack();

   void setStopFlag(bool s) noexcept { stop_flag.store(s); }
   bool isStopFlag() const noexcept { return stop_flag.load(); }

   void setSkipFlag(bool s) noexcept { skip_flag.store(s); }
   bool isSkipFlag() const noexcept { return skip_flag.load(); }

   void setBackFlag(bool s) noexcept { back_flag.store(s); }
   bool isBackFlag() const noexcept { return back_flag.load(); }

   void startPlayer(dpp::voiceconn *v);

   size_t getCurrentIndex();

   bool isHistoryEmpty();
   bool isPlaylistEmpty();
   bool HasNextTrack();

   void updateWorkingStreamLink(Track &track);

   void clear();
   size_t size();
};