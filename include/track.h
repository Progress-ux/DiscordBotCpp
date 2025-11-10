#pragma once

#include <string>

class Track
{
private:
   inline static const std::string BEGIN_URL = "https://youtu.be/"; 

   std::string author = "Unknown";
   std::string title = "Unknown";
   std::string duration = "Unknown";
   std::string url;
   std::string stream_url;

public:
   void setAuthor(const std::string& author);
   void setTitle(const std::string& title);
   void setUrl(const std::string& url);
   void setStreamUrl(const std::string& stream_url);
   void setDuration(const std::string& duration);

   const std::string& getAuthor() const { return author; }
   const std::string& getTitle() const { return title; }
   const std::string& getUrl() const { return url; }
   const std::string& getStreamUrl() const { return stream_url; }
   const std::string& getDuration() const { return duration; }
   static const std::string& getBeginUrl() { return BEGIN_URL; }

   std::string getInfo();
};