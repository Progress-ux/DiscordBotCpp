#pragma once

#include "track.h"
#include <vector>
#include <optional>
#include <mutex>

class MusicHandler
{
private:
   std::vector<Track> playlist;
   size_t current_index = 0;
   mutable std::mutex mtx;

public:
enum class State { Idle, Playing, Paused };

private:
   State state = State::Idle;

public:
   void addTrack(Track track);
   void removeTrack(size_t index);

   std::optional<Track> getCurrentTrack() const;
   std::optional<Track> getNextTrack();

   bool isEmpty();
   void clear();
   size_t size();

   void setState(State state);
   State getState() const
   {
      std::lock_guard<std::mutex> lock(mtx);   
      return state;
   }
};