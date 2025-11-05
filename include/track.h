#pragma once

#include <string>

class Track
{
private:
   std::string author = "Unknown";
   std::string title = "Unknown";
   std::string url;
   std::string duration = "Unknown";

public:
   void setAuthor(std::string& author);
   void setTitle(std::string& title);
   void setUrl(std::string& url);
   void setDuration(std::string& duration);

   const std::string& getAuthor() { return author; }
   const std::string& getTitle() { return title; }
   const std::string& getUrl() { return url; }
   const std::string& getDuration() { return duration; }

};