#include "musichandler.h"
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <iostream>

extern "C" {
   #include <libavformat/avformat.h>
   #include <libavcodec/codec.h>
   #include <libavcodec/avcodec.h>
   #include <libswresample/swresample.h>
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

void MusicHandler::playTrack(std::string url, dpp::voiceconn *v)
{
   FILE* ffmpeg = popen(
      ("ffmpeg -re -i \"" + url + "\" -f s16le -ar 48000 -ac 2 pipe:1 2>/dev/null").c_str(),
      "r"
   );

   std::vector<uint8_t> buf(11520);
   while (true) {
      while (v->voiceclient->is_paused()) 
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      size_t bytes_read = fread(buf.data(), 1, buf.size(), ffmpeg);
      if (bytes_read == 0) break;

      if (bytes_read < buf.size())
         std::fill(buf.begin() + bytes_read, buf.end(), 0);

      v->voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(buf.data()), buf.size());
   }

   pclose(ffmpeg);
}
