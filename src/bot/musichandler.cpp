#include "musichandler.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <iostream>
#include <regex>
#include <dpp/utility.h>
#include <cmath>

void MusicHandler::startPlayer()
{
   std::lock_guard<std::mutex> lock(player_mutex);
   if (is_playing.load()) return; 
   is_playing.store(true);
   this->Player();
}

Track &MusicHandler::getTrackFromHistory(size_t index)
{
   if(index > history.size())
      throw std::out_of_range("The index has gone beyond history");
   return history.at(index);
}


Track &MusicHandler::getTrackFromQueue(size_t index)
{
   if(index > queue.size())
      throw std::out_of_range("The index has gone beyond queue");
   return queue.at(index);
}

void MusicHandler::addTrack(std::string &url)
{
   try
   {
      std::lock_guard<std::mutex> guard (command_mutex);

      if(!Utils::isValidUrl(url))
         throw std::runtime_error("Invalid link entered!");

      Track track = Utils::extractInfo(url);
      
      if(track.empty())
         throw std::runtime_error("Failed to retrieve information!");

      queue.push_back(track);
   }
   catch(const std::exception& e)
   {
      throw std::runtime_error(e.what());
   }
}

bool MusicHandler::isHistoryEmpty()
{
   std::lock_guard<std::mutex> lock(command_mutex);
   return history.empty();
}

bool MusicHandler::isQueueEmpty()
{
   std::lock_guard<std::mutex> lock(command_mutex);
   return queue.empty();
}

void MusicHandler::clear()
{
   queue.clear();
}

size_t MusicHandler::queueSize()
{
   return queue.size();
}

size_t MusicHandler::historySize()
{
   return history.size();
}


Track& MusicHandler::getCurrentTrack() 
{
   if(current_track.empty())
      current_track = this->getNextTrack();
   return current_track;
}

Track& MusicHandler::getLastTrack()
{
   return queue.back();
}

Track& MusicHandler::getNextTrack() 
{
   if(isQueueEmpty())
   {
      current_track = Track();
      return current_track;
   }

   // Move the current track to history before updating
   if(!current_track.empty())
      history.push_front(current_track);

   current_track = queue.front();
   queue.pop_front();
   return current_track;
}

Track &MusicHandler::getBackTrack()
{
   if(isHistoryEmpty())
   {
      current_track = Track();
      return current_track;
   }

   // Move the current track back to the front of the queue
   if(!current_track.empty())
      queue.push_front(current_track);

   current_track = history.front();
   history.pop_front();
   return current_track;
}

void MusicHandler::Player()
{
   while(true)
   {
      try
      {
         if(!voiceclient || isStopFlag())
            break;
            
         if(back_flag.load())
         {
            back_flag.store(false);

            if(isHistoryEmpty())
               break;
            
            Utils::updateWorkingStreamLink(getBackTrack());
         }
         else 
         {
            skip_flag.store(false);

            if(isQueueEmpty())
               break;
            
            Utils::updateWorkingStreamLink(getNextTrack());
         }
         playTrack(getCurrentTrack().getStreamUrl());
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
         break;
      }
   }
   is_playing.store(false);
   stop_flag.store(false);
}

void MusicHandler::playTrack(std::string stream_url)
{
   if(stream_url.empty())
      return;

   std::string cmd = "ffmpeg"  
                     " -reconnect 1 -reconnect_streamed 1 -reconnect_delay_max 2" 
                     " -re -i \"" 
                     + stream_url + 
                     "\" -f s16le -ar 48000 -ac 2 pipe:1 2>/dev/null";

   std::unique_ptr<FILE, decltype(&pclose)> ffmpeg(
      popen(cmd.c_str(), "r"),
      pclose
   );

   if (!ffmpeg) 
   {
      std::cerr << "Failed to start FFmpeg\n";
      return;
   }

   u_int i = 0;
   std::vector<uint8_t> buf(11520);
   while (!stop_flag.load()) 
   {
      while (voiceclient && voiceclient->is_paused())
         std::this_thread::sleep_for(std::chrono::milliseconds(100));

      size_t bytes_read = fread(buf.data(), 1, buf.size(), ffmpeg.get());
      if (bytes_read == 0) 
      {
         if (feof(ffmpeg.get())) break;

         if (ferror(ffmpeg.get())) 
         {
            std::cerr << "Error reading FFmpeg output\n";
            break;
         }
      }

      if (bytes_read < buf.size())
         std::fill(buf.begin() + bytes_read, buf.end(), 0);

      if(skip_flag.load() || back_flag.load()) break;
      if(stop_flag.load() || disconnect_flag.load()) break;
      
      if(voiceclient && voiceclient->is_connected())
         voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(buf.data()), buf.size());
      else 
      {
         if(i >= 5)
         {
            voiceclient = nullptr;
            break;
         }
         ++i;
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
         continue; 
      }
   }

   if(voiceclient)
      voiceclient->stop_audio();
}

