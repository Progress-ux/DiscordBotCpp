#include "musichandler.h"
#include <stdexcept>

void MusicHandler::addTrack(Track track)
{
   std::lock_guard<std::mutex> lock(mtx);   
   playlist.push_back(track);
}

void MusicHandler::removeTrack(size_t index)
{
   std::lock_guard<std::mutex> lock(mtx);   
   if(index > playlist.size())
      throw std::out_of_range("Invalid track index");

   playlist.erase(playlist.begin() + index);
   if (current_index >= playlist.size() && !playlist.empty())
      current_index = playlist.size() - 1;
}

std::optional<Track> MusicHandler::getCurrentTrack() const
{
   std::lock_guard<std::mutex> lock(mtx);   
   if(playlist.empty()) return std::nullopt;
   return playlist[current_index];
}

std::optional<Track> MusicHandler::getNextTrack()
{
   std::lock_guard<std::mutex> lock(mtx);   
   if(playlist.empty()) return std::nullopt;

   current_index = (current_index + 1) % playlist.size();
   return playlist[current_index];
}

bool MusicHandler::isEmpty()
{
   std::lock_guard<std::mutex> lock(mtx);   
   return playlist.empty(); 
}

void MusicHandler::clear()
{
   std::lock_guard<std::mutex> lock(mtx);   
   playlist.clear();
   current_index = 0;
}

size_t MusicHandler::size()
{
   std::lock_guard<std::mutex> lock(mtx);   
   return playlist.size();
}

void MusicHandler::setState(State state)
{
   std::lock_guard<std::mutex> lock(mtx);
   this->state = state;
}


