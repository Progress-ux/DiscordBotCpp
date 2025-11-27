#include "musichandler.h"
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <iostream>
#include <regex>
#include <dpp/utility.h>

std::string MusicHandler::addTrack(std::string& url)
{
   try
   {
      if(!isValidUrl(url))
         throw "Invalid link entered!";

      Track track = extractInfo(url);
      
      if(track.empty())
         throw "Failed to retrieve information!";

      queue.push_back(track);

      std::string duration = track.getDuration(); // in seconds
      int minutes = std::stoi(duration) / 60;
      int seconds = std::stoi(duration) % 60;

      std::string response = 
         "**Title:** " + track.getTitle() + "\n" +
         "**Artist:** " + track.getAuthor() + "\n" +
         "**Duration:** " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds) + "\n" +
         "**Queue Position:** " + std::to_string(queueSize());

      return response;
   }
   catch(const std::exception& e)
   {
      throw e.what();
   }
}

bool MusicHandler::isHistoryEmpty()
{
   return history.empty();
}

bool MusicHandler::isQueueEmpty()
{
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
   {
      current_track = queue.front();
      queue.pop_front();
   }

   return current_track;
}

Track& MusicHandler::getLastTrack()
{
   return queue.back();
}

Track& MusicHandler::getNextTrack() 
{
   current_track = queue.front();
   queue.pop_front();
   return current_track;
}

Track &MusicHandler::getBackTrack()
{
   current_track = history.front();
   history.pop_front();
   return current_track;
}

bool MusicHandler::isValidUrl(std::string &url)
{
   // const std::regex url_regex(
   //    R"(^(https)://[^\s/$.?#].[^\s]*$)"
   // );
   const std::regex url_regex(
      R"(^https?://[a-zA-Z0-9\-\._~:/?#\[\]@!#&'()*+,;=%]+$)"
   );
   return std::regex_match(url, url_regex);
}

void MusicHandler::updateWorkingStreamLink(Track &track)
{

   CURL *curl = curl_easy_init();
   
   if (!curl) 
      return;
   
   CURLcode res;
   long response_code = 0;

   curl_easy_setopt(curl, CURLOPT_URL, track.getStreamUrl().c_str());
   curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

   res = curl_easy_perform(curl);
   curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
   curl_easy_cleanup(curl);

   if (res != CURLE_OK || response_code >= 400)
      updateInfo(track);
}

void MusicHandler::updateInfo(Track &track)
{
   try
   {
      std::string yt_dlp_cmd =
               "yt-dlp "
               "-f bestaudio "
               "--dump-single-json "
               "--no-playlist "
               "--add-header \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36\" "
               "--add-header \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" "
               "--add-header \"Accept-Language: en-us,en;q=0.5\" "
               "--add-header \"Sec-Fetch-Mode: navigate\" "
               "\"" + track.getUrl() + "\" 2>/dev/null";
   
      FILE* yt_dlp = popen(yt_dlp_cmd.c_str(), "r");
      if(!yt_dlp)
      {
         std::cerr << "Cannot run yt-dlp" << std::endl;
         return;
      }
   
      std::string json_data;
      char buffer[1024];
      while (fgets(buffer, sizeof(buffer), yt_dlp)) {
         json_data += buffer;
      }
      pclose(yt_dlp);
   
      nlohmann::json result = nlohmann::json::parse(json_data);
   
      if (result.empty())
      {
         std::cerr << "Error: json empty" << std::endl;
      }
      
      track.setStreamUrl(result["url"]);
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
   }
}

Track MusicHandler::extractInfo(std::string &url)
{
   Track track;
   try
   {
      std::string yt_dlp_cmd =
               "yt-dlp "
               "-f bestaudio "
               "--dump-single-json "
               "--no-playlist "
               "--add-header \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36\" "
               "--add-header \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" "
               "--add-header \"Accept-Language: en-us,en;q=0.5\" "
               "--add-header \"Sec-Fetch-Mode: navigate\" "
               "\"" + url + "\" 2>/dev/null";
   
      FILE* yt_dlp = popen(yt_dlp_cmd.c_str(), "r");
      if(!yt_dlp)
         throw "Cannot run yt-dlp";
   
      std::string json_data;
      char buffer[1024];
      while (fgets(buffer, sizeof(buffer), yt_dlp)) {
         json_data += buffer;
      }
      pclose(yt_dlp);
   
      nlohmann::json result = nlohmann::json::parse(json_data);
   
      if (result.empty())
         throw "Error: json empty";
   
      // Stores track information
      track.setTitle(result["title"]);
      track.setAuthor(result["uploader"]);
      track.setUrl(track.getBeginUrl() + (std::string)result["id"]);
      track.setDuration(std::to_string(result.value("duration", 0)));
      track.setStreamUrl(result["url"]);
      return track;
   }
   catch(const std::exception& e)
   {
      throw e.what();
   }
   
}

void MusicHandler::Player()
{
   while(true)
   {
      try
      {
         if(!voice_client.lock() || isStopFlag())
            break;

         playTrack(getCurrentTrack().getStreamUrl());

         if(isBackFlag())
         {
            setBackFlag(false);
            updateWorkingStreamLink(getBackTrack());

            // The current track is added to the queue
            queue.push_front(current_track);
         }
         else if (isSkipFlag())
         {
            setSkipFlag(false);
            updateWorkingStreamLink(getNextTrack());
            
            // The current track is added to the beginning of the story
            history.push_front(current_track);
         }
         else 
         {
            if(isQueueEmpty())
               break;
               
            updateWorkingStreamLink(getNextTrack());
            
            // The current track is added to the beginning of the story
            history.push_front(current_track);
         }
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
         break;
      }
   }
   setStopFlag(false);
}

void MusicHandler::playTrack(std::string stream_url)
{
   FILE* ffmpeg = popen(
      ("ffmpeg"  
         " -reconnect 1 -reconnect_streamed 1 -reconnect_delay_max 2" 
         " -re -i \"" 
         + stream_url + 
         "\" -f s16le -ar 48000 -ac 2 pipe:1 2>/dev/null").c_str(),
      "r"
   );

   if (!ffmpeg) 
   {
      std::cerr << "Failed to start FFmpeg\n";
      return;
   }

   u_int i = 0;
   std::vector<uint8_t> buf(11520);
   while (!isStopFlag()) 
   {
      // while (v /* && pause_flag*/) // TODO: Add flags or structure
      //    std::this_thread::sleep_for(std::chrono::milliseconds(100));

      size_t bytes_read = fread(buf.data(), 1, buf.size(), ffmpeg);
      if (bytes_read == 0) 
      {
         if (feof(ffmpeg)) break;

         if (ferror(ffmpeg)) 
         {
            std::cerr << "Error reading FFmpeg output\n";
            break;
         }
      }

      if (bytes_read < buf.size())
         std::fill(buf.begin() + bytes_read, buf.end(), 0);

      if(isSkipFlag() || isBackFlag()) break;
      if(isStopFlag() || isDisconnectFlag()) break;
      
      if(auto vc = voice_client.lock())
      {
         if(vc->is_connected() && !vc->terminating)
            vc->send_audio_raw(reinterpret_cast<uint16_t*>(buf.data()), buf.size());
         else 
            break;
      } 
      else 
      {
         if(i >= 5)
            break;
         ++i;
         std::cerr << "Voice client destroyed from " << guild_id << ", cannot send audio. Attempt: " << i << "\n"; 
         std::this_thread::sleep_for(std::chrono::seconds(1));
         continue; 
      }
   }
   pclose(ffmpeg);
}

