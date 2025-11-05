#include "play_command.h"
#include <ogg/ogg.h>
#include <opusfile/include/opusfile.h>
#include <string>
#include <nlohmann/json.hpp>


void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   musicHandler = bot.getMusicHandler(event.command.guild_id);
   dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);

   if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
      event.reply("There was an issue with getting the voice channel. Make sure I'm in a voice channel!");
      return;
   }
   auto val = event.get_parameter("url"); // возвращает command_value
   std::string val_url = std::get<std::string>(val);
   
   std::string json_cmd = "yt-dlp -J --no-playlist " + val_url + " 2>/dev/null";

   std::string json_data;   

   FILE* yt_dlp = popen(json_cmd.c_str(), "r");
   char buffer[256];
   while(fgets(buffer, sizeof(buffer), yt_dlp)) json_data += buffer;
   pclose(yt_dlp);

   nlohmann::json data = nlohmann::json::parse(json_data);

   
   Track track;
   track.setAuthor((std::string)data["uploader"]);
   track.setTitle((std::string)data["title"]);
   track.setUrl(track.getBeginUrl() + (std::string)data["id"]);
   track.setDuration((std::string)data["duration_string"]);
   musicHandler.addTrack(track);

   event.reply("Play: " + track.getTitle());
   std::string play_cmd =
                "yt-dlp -f bestaudio -o - " + track.getUrl() +
                " 2>/dev/null | ffmpeg -i pipe:0 -c:a libopus -b:a 128k -ar 48000 -ac 2 -f ogg pipe:1 2>/dev/null";
   FILE* ffmpeg = popen(play_cmd.c_str(), "r");
   if (!ffmpeg) {
         std::cerr << "Ошибка запуска ffmpeg/yt-dlp\n";
         return;
   }

   ogg_sync_state oy;
   ogg_stream_state os;
   ogg_page og;
   ogg_packet op;
   OpusHead header;

   ogg_sync_init(&oy);

   char* buf;
   int bytes;

   while (true)
   {
      buf = ogg_sync_buffer(&oy, 4096);
      bytes = fread(buf, 1, 4096, ffmpeg);
      if (bytes == 0) break;
      ogg_sync_wrote(&oy, bytes);

      while (ogg_sync_pageout(&oy, &og) == 1)
      {
         ogg_stream_init(&os, ogg_page_serialno(&og));
         ogg_stream_pagein(&os, &og);

         while(ogg_stream_packetout(&os, &op) == 1)
         {
            if(op.bytes > 8 && !memcmp("OpusHead", op.packet, 8))
               continue;
            if(op.bytes > 8 && !memcmp("OpusTags", op.packet, 8))
               continue;
            
            int samples = opus_packet_get_samples_per_frame(op.packet, 48000);
            v->voiceclient->send_audio_opus(op.packet, op.bytes, samples / 48);
         }
         ogg_stream_clear(&os);
      }
   }
   ogg_sync_clear(&oy);
   pclose(ffmpeg);  
}