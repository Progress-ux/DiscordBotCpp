#pragma once

#include "track.hpp"
#include <dpp/dpp.h>
#include <atomic>
#include <vector>
#include <deque>
#include <mutex>
#include <memory>

/**
 * @class MusicHandler
 * @brief Manages the track queue, history, and playback logic.
 *
 * The class contains methods for adding and switching tracks, updating
 * working stream links via yt-dlp, as well as launching the main player loop.
 * Stores the current track, queue, history and a sstd::weak_ptr<discord_voice_client>et of control flags.
 */
class MusicHandler
{
private:
   dpp::snowflake guild_id;                   ///< Current server for the player

   std::mutex mutex;

   Track current_track;                       ///< Current playing track.
   
   std::deque<Track> queue;                   ///< Queue of tracks for playback.
   std::deque<Track> history;                 ///< History of recently played tracks.
   
   std::atomic<bool> stop_flag;               ///< Playback stop flag.
   std::atomic<bool> skip_flag;               ///< Flag for skipping the current track.
   std::atomic<bool> back_flag;               ///< Flag for moving to the previous track.
   std::atomic<bool> disconnect_flag;         ///< Completion flag when the bot is disconnected.
   
   /**
    * @brief Plays the audio stream through FFmpeg and sends PCM data to the voice channel.
    *
    * Launches FFmpeg, reads the stream and sends it to voiceclient->send_audio_raw().
    * Execution stops when the Stop/Skip/Back/Disconnect flags are triggered.
    *
    * @param stream_url Direct link to the audio stream.
    * @param v DPP voice connection.
    */
   void playTrack(std::string stream_url);
   
public:
   std::shared_ptr<dpp::discord_voice_client> voiceclient;
   /**
    * @brief The constructor that accepts for writing the server with which it works 
    * 
    * @param _bot Pointer to bot for permanent access to voiceconn
    * @param _guild_id ID of the server to which it will be linked
    */
   MusicHandler(dpp::snowflake _guild_id) : guild_id(_guild_id) { voiceclient = nullptr; }

   /**
    * @brief Adds a new track to the queue.
    *
    * @param url Video link.
    * @return response: title, uploader, duration(formatted), queue position  
    */
   void addTrack(std::string &url);

   /**
    * @brief Retrieves track information via yt-dlp.
    *
    * Receives JSON metadata, parses it and generates a Track object:
    * title, author, duration, URL and working stream link.
    *
    * @param url Source URL of the track.
    * @return The populated Track structure.
    */
   Track extractInfo(std::string& url);

   /**
    * @brief Updates the working stream link for the track.
    *
    * Used when the old link is not available. Requeries data via yt-dlp.
    *
    * @param track The track for which the update is required.
    */
   void updateInfo(Track &track);

   /**
    * @brief Returns the current track.
    *
    * If there is no current track, it automatically retrieves the next one.
    *
    * @return Link to the current track.
    */
   Track& getCurrentTrack();

   /**
    * @brief Returns the last track added to the queue.
    *
    * @warning The queue must be non-empty.
    *
    * @return Link to the last track.
    */
   Track& getLastTrack();

   /**
    * @brief Retrieves the next track from the queue.
    *
    * Makes it current.
    *
    * @warning The queue must be non-empty.
    *
    * @return Link to the new current track.
    */
   Track& getNextTrack();

   /**
    * @brief Retrieves the previous track from the history.
    *
    * @warning History must be non-empty.
    *
    * @return Link to the previous track.
    */
   Track& getBackTrack();

   void setVoiceClient(std::shared_ptr <dpp::discord_voice_client> _voice_client) { voiceclient = _voice_client; }

   /// Setting and receiving the stop flag.
   void setStopFlag(bool s) noexcept { stop_flag.store(s); }
   bool isStopFlag() const noexcept { return stop_flag.load(); }

   /// Setting and receiving the skip flag.
   void setSkipFlag(bool s) noexcept { skip_flag.store(s); }
   bool isSkipFlag() const noexcept { return skip_flag.load(); }

   /// Setting and getting the step back flag.
   void setBackFlag(bool s) noexcept { back_flag.store(s); }
   bool isBackFlag() const noexcept { return back_flag.load(); }

   /// Set and get the shutdown flag.
   void setDisconnectFlag(bool s) noexcept { disconnect_flag.store(s); }
   bool isDisconnectFlag() const noexcept { return disconnect_flag.load(); }

   /**
    * @brief Main playback loop.
    *
    * Plays tracks one by one, processing control flags
    * (Stop, Skip, Back, Disconnect). Switches tracks and updates their working links.
    *
    * @param v DPP voice connection.
    */
   void Player();

   /**
    * @brief Checks if the history is empty.
    * @return true if history is empty.
    */
   bool isHistoryEmpty();

   /**
    * @brief Checks if the queue is empty.
    * @return true if the queue is empty.
    */
   bool isQueueEmpty();

   /**
    * @brief Formats the duration in seconds into a MM:SS or HH:MM::SS format string
    * @param sec Duration in seconds (integer)
    * @return Formatted duration string
    */
   std::string formatDuration(const std::string& sec_str);

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
   void updateWorkingStreamLink(Track &track);

   /**
    * @brief Clears the queue completely.
    */
   void clear();

   /**
    * @brief Returns the number of tracks in the queue.
    * @return Queue size.
    */
   size_t queueSize();

   /**
    * @brief Returns the number of tracks in the history.
    * @return History size.
    */
   size_t historySize();
};