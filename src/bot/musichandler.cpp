#include "musichandler.hpp"
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

      if(!isValidUrl(url))
         throw std::runtime_error("Invalid link entered!");

      Track track = extractInfo(url);
      
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

std::string MusicHandler::formatDuration(const std::string& sec_str)
{
   long long sec;

   // Trying to convert the input string to a number
   try {
      sec = std::stoll(sec_str);
   } catch (const std::invalid_argument& e) {
      // If the string does not contain a number, return a dash
      return "—";
   } catch (const std::out_of_range& e) {
      // If the number is too big for long long
      return "—";
   }

   if (sec <= 0) {
      return "—";
   }

   const long long h = std::floor(sec / 3600);
   const long long m = std::floor((sec % 3600) / 60);
   const long long s = sec % 60;

   // Use stringstream for zero padding formatting
   std::stringstream ss;
   
   if (h > 0) {
      ss << h << ":";
      // std::setw(2) sets the field width to 2 characters
      // std::setfill('0') fills the empty space with zeros
      ss << std::setw(2) << std::setfill('0') << m << ":";
      ss << std::setw(2) << std::setfill('0') << s;
   } else {
      ss << m << ":";
      ss << std::setw(2) << std::setfill('0') << s;
   }

   return ss.str();
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
      // std::string path_from_cookies = "/home/container/cookies.txt";      
      std::string yt_dlp_cmd =
               "/home/container/.local/bin/yt-dlp "
               "-f bestaudio/best "
               "--dump-single-json "
               "--no-playlist "
               "--quiet "
               "--no-warnings "
               "--extractor-args \"youtube:player_client=default\" "
               "--add-header \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36\" "
               "--add-header \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" "
               "--add-header \"Accept-Language: en-us,en;q=0.5\" "
               "--add-header \"Sec-Fetch-Mode: navigate\" "
               "\"" + track.getUrl() + "\" 2>/dev/null";
   
      std::unique_ptr<FILE, decltype(&pclose)> yt_dlp(
         popen(yt_dlp_cmd.c_str(), "r"),
         pclose
      );
      if(!yt_dlp)
      {
         std::cerr << "Cannot run yt-dlp" << std::endl;
         return;
      }
   
      std::string json_data;
      char buffer[1024];
      while (fgets(buffer, sizeof(buffer), yt_dlp.get())) {
         json_data += buffer;
      }
   
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
      // std::string path_from_cookies = "/home/container/cookies.txt";      
      std::string yt_dlp_cmd =
               "/home/container/.local/bin/yt-dlp "
               "-f bestaudio/best "
               "--dump-single-json "
               "--no-playlist "
               "--quiet "
               "--no-warnings "
               "--extractor-args \"youtube:player_client=default\" "
               "--add-header \"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36\" "
               "--add-header \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" "
               "--add-header \"Accept-Language: en-us,en;q=0.5\" "
               "--add-header \"Sec-Fetch-Mode: navigate\" "
               "\"" + url + "\" 2>/dev/null";

      std::unique_ptr<FILE, decltype(&pclose)> yt_dlp(
         popen(yt_dlp_cmd.c_str(), "r"),
         pclose
      );
      if(!yt_dlp)
         throw std::runtime_error("Cannot run yt-dlp");
   
      std::string json_data;
      char buffer[1024];
      while (fgets(buffer, sizeof(buffer), yt_dlp.get())) {
         json_data += buffer;
      }

      if (json_data.empty())
         throw std::runtime_error("yt-dlp returned empty output");
   
      nlohmann::json result = nlohmann::json::parse(json_data);
   
      if (result.empty())
         throw std::runtime_error("json empty");
   
      // Stores track information
      track.setTitle(result["title"]);
      track.setAuthor(result["uploader"]);
      track.setUrl(track.getBeginUrl() + (std::string)result["id"]);
      track.setDuration(formatDuration(std::to_string(result.value("duration", 0))));
      track.setStreamUrl(result["url"]);
      track.setThumbnail(result["thumbnail"]); 
      return track;
   }
   catch(const std::exception& e)
   {
      throw std::runtime_error(e.what());
   }
   
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
            
            updateWorkingStreamLink(getBackTrack());
         }
         else 
         {
            skip_flag.store(false);

            if(isQueueEmpty())
               break;
            
            updateWorkingStreamLink(getNextTrack());
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

