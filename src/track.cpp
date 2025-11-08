#include "track.h"

void Track::setAuthor(const std::string& author) 
{
   this->author = author;
}

void Track::setTitle(const std::string& title)
{
   this->title = title;
}

void Track::setUrl(const std::string& url)
{
   this->url = url;
}

void Track::setDuration(const std::string& duration)
{
   this->duration = duration;
}

std::string Track::getInfo()
{
   std::string info = author + "\n" +
                      title + "\n" +
                      duration + "\n";
   return info;
}
