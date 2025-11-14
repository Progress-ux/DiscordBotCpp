#include "musichandler.h"
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <iostream>

void MusicHandler::addTrack(Track track)
{
   playlist.push_back(track);
}

bool MusicHandler::isHistoryEmpty()
{
   return current_index == 0;
}

bool MusicHandler::isPlaylistEmpty()
{
   return playlist.empty();
}

bool MusicHandler::HasNextTrack()
{
   return current_index < playlist.size();
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
      extractInfo(track);
}

void MusicHandler::clear()
{
   playlist.clear();
}

size_t MusicHandler::size()
{
   return playlist.size();
}

size_t MusicHandler::getCurrentIndex()
{
   return current_index;
}

Track& MusicHandler::getCurrentTrack() 
{
   if (playlist.empty() || current_index >= playlist.size())
      throw std::out_of_range("Playlist is empty or current_index out of range (getCurrentTrack)");

   return playlist[current_index];
}

Track& MusicHandler::getLastTrack()
{
   if (playlist.empty())
      throw std::out_of_range("Playlist is empty or current_index out of range (getLastTrack)");

   return playlist[playlist.size()-1];
}

Track& MusicHandler::getNextTrack() 
{
   if(current_index + 1 >= playlist.size())
      throw std::out_of_range("Playlist is empty or current_index out of range (getNextTrack)");

   ++current_index;
   return playlist[current_index];
}

Track &MusicHandler::getBackTrack()
{
   if(playlist.size() <= 1)
      throw std::out_of_range("History is empty or current_index out of range (getBackTrack)");

   --current_index;
   return playlist[current_index];
}

void MusicHandler::extractInfo(Track &track)
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

void MusicHandler::extractInfo(std::string &url)
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
               "\"" + url + "\" 2>/dev/null";
   
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
   
      // Хранит информацию о треке
      Track track;
      track.setTitle(result["title"]);
      track.setAuthor(result["uploader"]);
      track.setUrl(track.getBeginUrl() + (std::string)result["id"]);
      track.setDuration(std::to_string(result.value("duration", 0)));
      track.setStreamUrl(result["url"]);
      
      playlist.push_back(track);
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
   }
   
}

void MusicHandler::startPlayer(dpp::voiceconn *v)
{
   while(v->voiceclient->is_ready())
   {
      try
      {
         if(isPlaylistEmpty())
            break;

         playTrack(getCurrentTrack().getStreamUrl(), v);

         if(isBackFlag())
         {
            setBackFlag(false);
            extractInfo(getBackTrack());
         }
         else 
            extractInfo(getNextTrack());
      }
      catch(const std::exception& e)
      {
         std::cerr << e.what() << '\n';
         break;
      }
   }
}

void MusicHandler::playTrack(std::string stream_url, dpp::voiceconn *v)
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

   std::vector<uint8_t> buf(11520);
   while (true) 
   {
      if (!v || !v->voiceclient) break;
      while (v->voiceclient->is_paused())
      {
         if (!v || !v->voiceclient) break;
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

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

      if(isSkipFlag())
      {
         setSkipFlag(false);
         break;
      }

      if(isBackFlag())
         break;

      if(isStopFlag())
      {
         setStopFlag(false);
         pclose(ffmpeg);
         return;
      }

      if (!v || !v->voiceclient)
         break;

      v->voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(buf.data()), buf.size());
   }

   // std::this_thread::sleep_for(std::chrono::seconds(5));
   if(ffmpeg)
      pclose(ffmpeg);
   
   if (v && v->voiceclient && v->voiceclient->is_ready())
      v->voiceclient->stop_audio();
}

