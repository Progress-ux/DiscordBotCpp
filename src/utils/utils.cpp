#include "utils.hpp"
#include <iostream>
#include <regex>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "core/log_macros.hpp"

bool Utils::isValidUrl(std::string &url)
{
   const std::regex url_regex(
      R"(^https?://[a-zA-Z0-9\-\._~:/?#\[\]@!#&'()*+,;=%]+$)"
   );
   return std::regex_match(url, url_regex);
}

void Utils::updateWorkingStreamLink(Track &track) 
{
   CURL *curl = curl_easy_init();
   
   if (!curl) 
   {
      LOG_ERROR("CURL unavailable");
      return;
   }
   
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
   {
      LOG_DEBUG("Update stream link, response code: " + response_code);
      updateInfo(track);
   }
}


void Utils::updateInfo(Track &track)
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
         LOG_ERROR("Cannot run yt-dlp");
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
         LOG_ERROR("Error: json empty");
         return;
      }
      
      track.setStreamUrl(result["url"]);
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
   }
}

Track Utils::extractInfo(std::string &url)
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

std::string Utils::formatDuration(const std::string &sec_str)
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

