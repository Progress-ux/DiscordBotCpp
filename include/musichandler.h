#pragma once

#include "track.h"
#include <vector>
#include <optional>

class MusicHandler
{
private:
   std::vector<Track> playlist;
   size_t current_index = 0;

public:
   void addTrack(Track track);
   void removeTrack(size_t index);

   std::optional<Track> getCurrentTrack() const;
   std::optional<Track> getNextTrack();

   bool isEmpty();
   void clear();
   size_t size();
};