#pragma once

#include "track.h"
#include <dpp/dpp.h>
#include <optional>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>

class MusicHandler
{
private:
   std::queue<Track> queue;
   std::queue<Track> history;
   mutable std::mutex mtx;

   bool is_playing = false;
   
   bool stop_flag;
   std::thread player_thread;
   
public:
   MusicHandler();

   void addTrack(Track track);

   void extractInfo(MusicHandler& musichandler, std::string& url);

   std::optional<Track> getCurrentTrack() const;
   Track getNextTrack();

   bool isEmpty();

   void clear();
   size_t size();

   void setIsPlaying(bool is_playing);
   bool getIsPlaying();
};