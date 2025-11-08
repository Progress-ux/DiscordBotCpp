#include "play_command.h"
#include <ogg/ogg.h>
#include <opusfile.h>
#include <string>
#include <nlohmann/json.hpp>

PlayCommand::PlayCommand(Bot &b) : bot(b) {}

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   try {
      dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);
      
      // Плеер по guild_id сервера
      // Хранит историю треков
      auto& musicHandler = bot.getMusicHandler(event.command.guild_id);
      musicHandler.setState(MusicHandler::State::Playing);

      if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
         event.edit_response("❌ Ошибка: я не в голосовом канале!");
         musicHandler.setState(MusicHandler::State::Idle);
         return;
      }
      
      auto val = event.get_parameter("url");
      std::string val_url = std::get<std::string>(val);
      
      // Загрузка стрим ссылки для воспроизведения
      std::string yt_dlp_cmd = "python3 worker.py " + val_url;
      FILE* yt_dlp = popen(yt_dlp_cmd.c_str(), "r");
      if(!yt_dlp)
      {
         std::cerr << "Cannot start worker.py" << std::endl;
         return;
      }

      std::string json_data;
      char buffer[1024];
      while (fgets(buffer, sizeof(buffer), yt_dlp)) {
         json_data += buffer;
      }
      pclose(yt_dlp);

      // Данные полученные из worker.py 
      nlohmann::json result = nlohmann::json::parse(json_data);

      // Хранит информацию о треке
      Track track;
      track.setTitle(result["title"]);
      track.setAuthor(result["author"]);
      track.setUrl(val_url);
      track.setDuration(std::to_string(result.value("duration", 0)));


      musicHandler.addTrack(track);
      event.edit_response("▶️ Воспроизвожу: **" + track.getTitle() + "**");

      std::string ffmpeg_cmd =
         "ffmpeg -i \"" + std::string(result["stream_url"]) +
         "\" -c:a libopus -b:a 128k -ar 48000 -ac 2 -f ogg pipe:1 2>/dev/null";

      // Воспроизведение в голосовой канал в opus формате
      FILE* ffmpeg = popen(ffmpeg_cmd.c_str(), "r");
      if (!ffmpeg) {
         event.edit_response("❌ Ошибка запуска ffmpeg");
         musicHandler.setState(MusicHandler::State::Idle);
         return;
      }

      ogg_sync_state oy;
      ogg_stream_state os;
      ogg_page og;
      ogg_packet op;

      ogg_sync_init(&oy);
      bool stream_initialized = false;

      while (true) {
         char* buf = ogg_sync_buffer(&oy, 4096);
         int bytes = fread(buf, 1, 4096, ffmpeg);
         if (bytes == 0) break;
         ogg_sync_wrote(&oy, bytes);

         while (ogg_sync_pageout(&oy, &og) == 1) {
            if (!stream_initialized) {
                  ogg_stream_init(&os, ogg_page_serialno(&og));
                  stream_initialized = true;
            }

            ogg_stream_pagein(&os, &og);

            while (ogg_stream_packetout(&os, &op) == 1) {
                  if (op.bytes > 8 && !memcmp("OpusHead", op.packet, 8))
                     continue;
                  if (op.bytes > 8 && !memcmp("OpusTags", op.packet, 8))
                     continue;

                  int samples = opus_packet_get_samples_per_frame(op.packet, 48000);
                  v->voiceclient->send_audio_opus(op.packet, op.bytes, samples / 48);
            }
         }
      }

      ogg_stream_clear(&os);
      ogg_sync_clear(&oy);

      pclose(ffmpeg);

      musicHandler.setState(MusicHandler::State::Idle);
   } catch (const std::exception& e) {
      event.edit_response(std::string("❌ Ошибка: ") + e.what());
   }
}
