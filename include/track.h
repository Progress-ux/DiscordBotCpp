#pragma once

#include <string>

class Track
{
private:
   const std::string BEGIN_URL = "https://youtu.be/"; 

   std::string author = "Unknown";
   std::string title = "Unknown";
   std::string url;
   std::string duration = "Unknown";

public:
   void setAuthor(const std::string& author);
   void setTitle(const std::string& title);
   void setUrl(const std::string& url);
   void setDuration(const std::string& duration);

   const std::string& getAuthor() { return author; }
   const std::string& getTitle() { return title; }
   const std::string& getUrl() { return url; }
   const std::string& getDuration() { return duration; }
   const std::string& getBeginUrl() { return BEGIN_URL; }
};