#include "musichandler.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <iostream>
#include <regex>
#include <dpp/utility.h>
#include <cmath>
#include "core/log_macros.hpp"

void MusicHandler::startPlayer()
{
   {
      std::lock_guard<std::mutex> lock(player_mutex);
      if (is_playing.load())
      {
         LOG_DEBUG("Player already running");
         return; 
      }
      LOG_INFO("Starting music player");
      is_playing.store(true);
   }
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

void MusicHandler::addTrackByLink(std::string &url)
{
   try
   {
      std::lock_guard<std::mutex> guard (command_mutex);

      LOG_DEBUG("Adding track from url");

      if(!Utils::isValidUrl(url))
      {
         LOG_WARN("Invalid url: " + url);
         throw std::runtime_error("Invalid link entered!");
      }

      Track track = Utils::extractInfo(url);
      
      if(track.empty())
      {
         LOG_ERROR("Track extraction failed");
         throw std::runtime_error("Failed to retrieve information!");
      }

      queue.push_back(track);
      LOG_DEBUG("Track added to queue");
   }
   catch(...)
   {
      throw;
   }
}

void MusicHandler::addTrackByQuery(std::string &query)
{
   try
   {
      std::lock_guard<std::mutex> guard (command_mutex);

      LOG_DEBUG("Adding track from query");

      Track track = Utils::extractInfoByName(query);
      
      if(track.empty())
      {
         LOG_ERROR("Track extraction failed");
         throw std::runtime_error("Failed to retrieve information!");
      }

      queue.push_back(track);
      LOG_DEBUG("Track added to queue");
   }
   catch(...)
   {
      throw;
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

void MusicHandler::clearAll()
{
   queue.clear();
   history.clear();
}

void MusicHandler::clearQueue()
{
   queue.clear();
}

void MusicHandler::clearHistory()
{
   history.clear();
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
   LOG_DEBUG("Player loop started");

   while(!stop_flag)
   {
      try
      {
         if(!voiceclient)
            break;
            
         if(back_flag)
         {
            LOG_DEBUG("Back track requested");
            back_flag.store(false);

            if(isHistoryEmpty())
            {
               LOG_DEBUG("History is empty, stopping player");
               break;
            }
            
            Utils::updateWorkingStreamLink(getBackTrack());
         }
         else if(skip_flag)
         {
            if(skip_flag)
            {
               LOG_DEBUG("Skip track requested");
               skip_flag.store(false);
            }

            if(isQueueEmpty())
            {
               LOG_DEBUG("Queue is empty, stopping player");
               break;
            }
            
            Utils::updateWorkingStreamLink(getNextTrack());
         }
         else if(is_repeat)
         {
            Utils::updateWorkingStreamLink(getCurrentTrack());
         }
         else
         {
            if(isQueueEmpty())
            {
               LOG_DEBUG("Queue is empty, stopping player");
               break;
            }
            
            Utils::updateWorkingStreamLink(getNextTrack());
         }
         if(voiceclient)
            playTrack(getCurrentTrack().getStreamUrl());

      }
      catch(const std::exception& e)
      {
         LOG_ERROR(std::string("Player error: ") + e.what());
         break;
      }
   }
   history.push_front(current_track);
   current_track = Track();

   LOG_DEBUG("Player loop stoped");

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
      LOG_ERROR("Failed to start FFmpeg");
      return;
   }

   u_int i = 0;
   std::vector<uint8_t> buf(11520);
   while (!stop_flag.load()) 
   {
      while (voiceclient && voiceclient->is_paused())
         std::this_thread::sleep_for(std::chrono::milliseconds(100));

      if(skip_flag.load() || back_flag.load()) break;
      if(stop_flag.load() || disconnect_flag.load()) break;
      
      size_t bytes_read = fread(buf.data(), 1, buf.size(), ffmpeg.get());
      if (bytes_read == 0) 
      {
         if (feof(ffmpeg.get())) break;

         if (ferror(ffmpeg.get())) 
         {
            LOG_ERROR("Error reading FFmpeg output");
            break;
         }
      }

      if (bytes_read < buf.size())
         std::fill(buf.begin() + bytes_read, buf.end(), 0);
      
      if(voiceclient && voiceclient->is_connected())
         voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(buf.data()), buf.size());
      else 
      {
         if(i >= 5)
         {
            voiceclient = nullptr;
            LOG_WARN("Voice client disconnected");
            break;
         }
         ++i;
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
         continue; 
      }
   }

   if(voiceclient)
   {
      LOG_DEBUG("Audio stream stopped");
      voiceclient->stop_audio();
   }
}