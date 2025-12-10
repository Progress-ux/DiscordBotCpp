#pragma once 
#include <string>
#include "track.hpp"

namespace Utils
{
   /**
    * @brief Checks the validity of a URL using a regular expression.
    *
    * @param url Link for verification.
    * @return true if the URL is valid.
    */
   bool isValidUrl(std::string &url);

   /**
    * @brief Checks the working stream link of a track via an HTTP request.
    *
    * If the link is not available, calls updateInfo() to get a new one.
    *
    * @param track Track to check.
    */
   void updateWorkingStreamLink(Track &track, long timeout_sec = 5);

   /**
    * @brief Formats the duration in seconds into a MM:SS or HH:MM::SS format string
    * @param sec Duration in seconds (integer)
    * @return Formatted duration string
    */
   std::string formatDuration(const std::string& sec_str);

   /**
    * @brief Updates the working stream link for the track.
    *
    * Used when the old link is not available. Requeries data via yt-dlp.
    *
    * @param track The track for which the update is required.
    */
   void updateInfo(Track &track);

   /**
    * @brief Retrieves track information via yt-dlp.
    *
    * Receives JSON metadata, parses it and generates a Track object:
    * title, author, duration, URL and working stream link.
    *
    * @param query video title.
    * @return The populated Track structure.
    */
   Track extractInfoByName(std::string &query);

   /**
    * @brief Retrieves track information via yt-dlp.
    *
    * Receives JSON metadata, parses it and generates a Track object:
    * title, author, duration, URL and working stream link.
    *
    * @param url Source URL of the track.
    * @return The populated Track structure.
    */
   Track extractInfo(std::string &url);
}