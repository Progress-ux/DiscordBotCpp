#include "musichandler.h"
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <iostream>
#include <ogg/ogg.h>
#include <opusfile.h>

MusicHandler::MusicHandler()
{
   stop_flag = false;
}

void MusicHandler::addTrack(Track track)
{
   queue.push(track);
}

bool MusicHandler::isEmpty()
{
   return queue.empty(); 
}

void MusicHandler::clear()
{
   std::queue<Track> empty;
   std::swap(queue, empty);
}

size_t MusicHandler::size()
{
   return queue.size();
}

void MusicHandler::setIsPlaying(bool is_playing)
{
   this->is_playing = is_playing;
}

bool MusicHandler::getIsPlaying()
{
   return is_playing;
}

void MusicHandler::extractInfo(MusicHandler &musicHandler, std::string &url)
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
         std::cerr << "Cannot start worker.py" << std::endl;
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
   
      
      queue.push(track);
      
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
   }
   
}

std::optional<Track> MusicHandler::getCurrentTrack() const
{
   if(queue.empty()) return std::nullopt;
   return queue.front();
}

Track MusicHandler::getNextTrack() 
{
   Track track = queue.front();
   queue.pop();
   return track;
}
