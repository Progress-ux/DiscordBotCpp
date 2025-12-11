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

   std::mutex command_mutex;                  ///< Command Queue Order 
   std::mutex player_mutex;                   ///< Playback order

   Track current_track;                       ///< Current playing track.
   
   std::deque<Track> queue;                   ///< Queue of tracks for playback.
   std::deque<Track> history;                 ///< History of recently played tracks.
   
   std::atomic<bool> stop_flag;               ///< Playback stop flag.
   std::atomic<bool> skip_flag;               ///< Flag for skipping the current track.
   std::atomic<bool> back_flag;               ///< Flag for moving to the previous track.
   std::atomic<bool> disconnect_flag;         ///< Completion flag when the bot is disconnected.
   std::atomic<bool> is_playing;              ///< Flag is playing
   std::atomic<bool> is_repeat;               ///< Flag is repeat
   
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

   /**
    * @brief Main playback loop.
    *
    * Plays tracks one by one, processing control flags
    * (Stop, Skip, Back, Disconnect). Switches tracks and updates their working links.
    */
   void Player();
   
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
    * Blocks the thread
    * If the track is playing, it exits
    * If not, then sets the is_playing flag to true and starts a new thread
    */
   void startPlayer();

   /**
    * @brief Returns the track by index
    * 
    * @param index track number in queue
    * @return link to track from queue
    */
   Track& getTrackFromHistory(size_t index);

   /**
    * @brief Returns the track by index
    * 
    * @param index track number in queue
    * @return link to track from queue
    */
   Track& getTrackFromQueue(size_t index);

   /**
    * @brief Adds a new track to the queue.
    *
    * @param url Video link.
    * @return response: title, uploader, duration(formatted), queue position  
    */
   void addTrackByLink(std::string &url);

   /**
    * @brief Adds a new track to the queue.
    *
    * @param query Video title.
    * @return response: title, uploader, duration(formatted), queue position  
    */
   void addTrackByQuery(std::string &query);

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

   std::deque<Track>& getHistory() { return history; }
   std::deque<Track>& getQueue() { return queue; }

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

   void setRepeatFlag(bool s) noexcept { is_repeat.store(s); }
   bool isRepeatFlag() const noexcept { return is_repeat.load(); }

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
    * @brief Completely clears history and queue.
    */
   void clearAll();

   /**
    * @brief Clears the queue completely.
    */
   void clearQueue();

   /**
    * @brief Clears the history completely.
    */
   void clearHistory();

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