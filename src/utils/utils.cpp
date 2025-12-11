#include "utils.hpp"
#include <iostream>
#include <regex>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "core/log_macros.hpp"
#include <vector>
#include <sys/wait.h>
#include <unistd.h>

bool Utils::isValidUrl(std::string &url)
{
   const std::regex url_regex(
      R"(^https?://[a-zA-Z0-9\-\._~:/?#\[\]@!#&'()*+,;=%]+$)"
   );
   return std::regex_match(url, url_regex);
}

void Utils::updateWorkingStreamLink(Track &track, long timeout_sec) 
{
   CURL *curl = curl_easy_init();
   
   if (!curl) 
   {
      LOG_ERROR("CURL unavailable");
      return;
   }
   
   CURLcode res;
   long http_code = 0;

   curl_easy_setopt(curl, CURLOPT_URL, track.getStreamUrl().c_str());
   curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);          
   curl_easy_setopt(curl, CURLOPT_RANGE, "0-0");        
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_sec);
   curl_easy_setopt(curl, CURLOPT_USERAGENT,
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
      +[](char*, size_t s, size_t n, void*) {
         return s * n;
      });

   res = curl_easy_perform(curl);

   if (res == CURLE_OK)
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

   curl_easy_cleanup(curl);

   if (res != CURLE_OK)
   {
      LOG_DEBUG(std::string("CURL error: ") + curl_easy_strerror(res));
      updateInfo(track);
      return;
   }

   if (http_code >= 400)
   {
      LOG_DEBUG("HTTP error code: " + std::to_string(http_code));
      updateInfo(track);
   }
}

void Utils::updateInfo(Track &track)
{
   try
   {
      int pipefd[2];
      pipe(pipefd);

      pid_t pid = fork();
      if(pid == 0)
      {
         dup2(pipefd[1], STDOUT_FILENO);
         dup2(pipefd[1], STDERR_FILENO);

         close(pipefd[0]);
         close(pipefd[1]);

         std::vector<std::string> str_args = {
            "/home/container/.local/bin/yt-dlp",
            "-f",
            "bestaudio/best",
            "--dump-single-json",
            "--no-playlist",
            "--quiet",
            "--no-warnings",
            "--extractor-args",
            "youtube:player_client=default",
            "--add-header",
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36",
            "--add-header",
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
            "--add-header",
            "Accept-Language: en-us,en;q=0.5",
            "--add-header",
            "Sec-Fetch-Mode: navigate",
            track.getUrl()
         };

         std::vector<char*> args;
         for(auto& s : str_args)
            args.push_back(s.data());
         args.push_back(nullptr);

         execvp("/home/container/.local/bin/yt-dlp", args.data());
         _exit(1);
      }
      close(pipefd[1]);
      
      std::string json_data;
      char buffer[4096];
      ssize_t n;

      while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
         json_data.append(buffer, n);

      close(pipefd[0]);
      int status;
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
      {
         LOG_ERROR("yt-dlp failed");
         return;
      }

      if (json_data.empty())
      {
         LOG_ERROR("yt-dlp returned empty output");
         return;
      }

      nlohmann::json result = nlohmann::json::parse(json_data);
   
      if (result.empty())
      {
         LOG_ERROR("Error: json empty");
         return;
      }
      
      track.setStreamUrl(result.value("url", ""));
   }
   catch(const std::exception& e)
   {
      LOG_ERROR(e.what());
   }
}

Track Utils::extractInfoByName(std::string &query)
{
   Track track;
   try
   {
      int pipefd[2];
      pipe(pipefd);

      pid_t pid = fork();
      if(pid == 0)
      {
         dup2(pipefd[1], STDOUT_FILENO);
         dup2(pipefd[1], STDERR_FILENO);

         close(pipefd[0]);
         close(pipefd[1]);

         std::vector<std::string> str_args = {
            "/home/container/.local/bin/yt-dlp",
            "-f",
            "bestaudio/best",
            "--dump-single-json",
            "--no-playlist",
            "--quiet",
            "--no-warnings",
            "--extractor-args",
            "youtube:player_client=default",
            "--add-header",
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36",
            "--add-header",
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
            "--add-header",
            "Accept-Language: en-us,en;q=0.5",
            "--add-header",
            "Sec-Fetch-Mode: navigate",
            "ytsearch1:" + query,
         };

         std::vector<char*> args;
         for(auto& s : str_args)
            args.push_back(s.data());
         args.push_back(nullptr);

         execvp("/home/container/.local/bin/yt-dlp", args.data());
         _exit(1);
      }
      close(pipefd[1]);
      
      std::string json_data;
      char buffer[4096];
      ssize_t n;

      while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
         json_data.append(buffer, n);

      close(pipefd[0]);
      int status;
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
         throw std::runtime_error("yt-dlp failed");

      if (json_data.empty())
         throw std::runtime_error("yt-dlp returned empty output");

      nlohmann::json result = nlohmann::json::parse(json_data);

      if (result.empty())
         throw std::runtime_error("json empty");

      auto entries = result.at("entries");
      if (entries.empty())
         throw std::runtime_error("No search results");

      auto& video = entries.at(0);

      // Stores track information
      track.setTitle(video.value("title", ""));
      track.setAuthor(video.value("uploader", ""));
      std::string id = video.value("id", "");
      if(!id.empty())
         track.setUrl(track.getBeginUrl() + id);
      track.setDuration(formatDuration(std::to_string(video.value("duration", 0))));
      track.setStreamUrl(video.value("url", ""));
      track.setThumbnail(video.value("thumbnail", ""));
   }
   catch(const std::exception& e)
   {
      LOG_ERROR(e.what());
   }
   return track;
}

Track Utils::extractInfo(std::string &url)
{
   Track track;
   try
   {
      int pipefd[2];
      pipe(pipefd);

      pid_t pid = fork();
      if(pid == 0)
      {
         dup2(pipefd[1], STDOUT_FILENO);
         dup2(pipefd[1], STDERR_FILENO);

         close(pipefd[0]);
         close(pipefd[1]);

         std::vector<std::string> str_args = {
            "/home/container/.local/bin/yt-dlp",
            "-f",
            "bestaudio/best",
            "--dump-single-json",
            "--no-playlist",
            "--quiet",
            "--no-warnings",
            "--extractor-args",
            "youtube:player_client=default",
            "--add-header",
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36",
            "--add-header",
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
            "--add-header",
            "Accept-Language: en-us,en;q=0.5",
            "--add-header",
            "Sec-Fetch-Mode: navigate",
            url
         };

         std::vector<char*> args;
         for(auto& s : str_args)
            args.push_back(s.data());
         args.push_back(nullptr);

         execvp("/home/container/.local/bin/yt-dlp", args.data());
         _exit(1);
      }
      close(pipefd[1]);
      
      std::string json_data;
      char buffer[4096];
      ssize_t n;

      while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
         json_data.append(buffer, n);

      close(pipefd[0]);
      int status;
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
         throw std::runtime_error("yt-dlp failed");

      if (json_data.empty())
         throw std::runtime_error("yt-dlp returned empty output");
   
      nlohmann::json result = nlohmann::json::parse(json_data);
   
      if (result.empty())
         throw std::runtime_error("json empty");
   
      // Stores track information
      track.setTitle(result.value("title", ""));
      track.setAuthor(result.value("uploader", ""));
      std::string id = result.value("id", "");
      if(!id.empty())
         track.setUrl(track.getBeginUrl() + id);
      track.setDuration(formatDuration(std::to_string(result.value("duration", 0))));
      track.setStreamUrl(result.value("url", ""));
      track.setThumbnail(result.value("thumbnail", "")); 
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

