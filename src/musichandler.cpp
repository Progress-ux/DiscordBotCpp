#include "musichandler.h"
#include <stdexcept>

void MusicHandler::addTrack(Track track)
{
   playlist.push_back(track);
}

void MusicHandler::removeTrack(size_t index)
{
   if(index > playlist.size())
      throw std::out_of_range("Invalid track index");

   playlist.erase(playlist.begin() + index);
   if (current_index >= playlist.size() && !playlist.empty())
      current_index = playlist.size() - 1;
}

std::optional<Track> MusicHandler::getCurrentTrack() const
{
   if(playlist.empty()) return std::nullopt;
   return playlist[current_index];
}

std::optional<Track> MusicHandler::getNextTrack()
{
   if(playlist.empty()) return std::nullopt;

   current_index = (current_index + 1) % playlist.size();
   return playlist[current_index];
}

bool MusicHandler::isEmpty()
{
   return playlist.empty(); 
}

void MusicHandler::clear()
{
   playlist.clear();
   current_index = 0;
}

size_t MusicHandler::size()
{
   return playlist.size();
}
