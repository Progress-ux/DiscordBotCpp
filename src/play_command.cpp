#include "play_command.h"
#include <string>
#include <vector>

extern "C" {
   #include <libavformat/avformat.h>
   #include <libavcodec/codec.h>
   #include <libavcodec/avcodec.h>
   #include <libswresample/swresample.h>
}


PlayCommand::PlayCommand(Bot &b) : bot(b) {}

void PlayCommand::execute(const dpp::slashcommand_t &event)
{
   event.thinking(); 

   try {
      dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);
      
      // Плеер по guild_id сервера
      // Хранит историю треков
      auto& musicHandler = bot.getMusicHandler(event.command.guild_id);

      if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
         event.edit_response("Error: I'm not in the voice channel!");
         return;
      }
      
      auto val = event.get_parameter("url");
      std::string val_url = std::get<std::string>(val);
      
      musicHandler.extractInfo(musicHandler, val_url);

      event.edit_response("**" + musicHandler.getCurrentTrack()->getTitle() + "**" + " added to queue");
      
      if(musicHandler.getIsPlaying())
         return;

         
      while(true)
      {
         if(musicHandler.isEmpty())
            break;
         
         if(musicHandler.getIsPlaying())
            continue;
            
         musicHandler.setIsPlaying(true);

         std::cout << "while" << std::endl;

         Track track; 
         track = musicHandler.getNextTrack();

         playTrack(track, v, musicHandler);
      }
      
   } 
   catch (const std::exception& e) 
   {
      event.edit_response(std::string("Error: ") + e.what());
   }
}

void PlayCommand::playTrack(Track &track, dpp::voiceconn *v, MusicHandler &musicHandler)
{
   try
   {
      std::cout << "playTrack()\n";

      AVFormatContext *fmt = nullptr;
      if (avformat_open_input(&fmt, track.getStreamUrl().c_str(), nullptr, nullptr) < 0)
         throw std::runtime_error("Cannot open stream");

      if (avformat_find_stream_info(fmt, nullptr) < 0)
         throw std::runtime_error("Cannot get stream info");

      int audio_stream = av_find_best_stream(fmt, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

      if (audio_stream < 0)
         throw std::runtime_error("No audio stream found");

      AVStream* stream = fmt->streams[audio_stream];

      const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
      if (!dec)
         throw std::runtime_error("Decoder not found");


      AVCodecContext *dec_ctx = avcodec_alloc_context3(dec);

      if (avcodec_parameters_to_context(dec_ctx, stream->codecpar) < 0)
         throw std::runtime_error("Failed to copy codec params");

      if (avcodec_open2(dec_ctx, dec, nullptr) < 0)
         throw std::runtime_error("Cannot open decoder");

      AVChannelLayout out_ch_layout;
      av_channel_layout_default(&out_ch_layout, 2);

      SwrContext *swr = nullptr; 
      if(swr_alloc_set_opts2(
         &swr,
         &out_ch_layout,
         AV_SAMPLE_FMT_S16,
         48000,
         &dec_ctx->ch_layout,
         dec_ctx->sample_fmt,
         dec_ctx->sample_rate,
         0,
         nullptr
      ) < 0)
         throw std::runtime_error("Failed to init swr");

      if (swr_init(swr) < 0)
         throw std::runtime_error("swr_init failed");

      AVPacket *pkt = av_packet_alloc();
      AVFrame *frame = av_frame_alloc();

      std::vector<uint8_t> pcm_buf;
      pcm_buf.reserve(11520);

      while (av_read_frame(fmt, pkt) >= 0)
      {
         if(pkt->stream_index != audio_stream)
         {
            av_packet_unref(pkt);
            continue;
         }

         avcodec_send_packet(dec_ctx, pkt);

         while (avcodec_receive_frame(dec_ctx, frame) >= 0)
         {
            int out_samples = av_rescale_rnd(
               swr_get_delay(swr, dec_ctx->sample_rate) + frame->nb_samples,
               48000,
               dec_ctx->sample_rate,
               AV_ROUND_UP
            );

            int out_buf_size = out_samples * 2 * sizeof(int16_t);

            std::vector<uint8_t> conv_buf(out_buf_size);
            uint8_t* out_planes[1] = { conv_buf.data() };

            swr_convert(
               swr, 
               out_planes, 
               out_samples, 
               (const uint8_t**)frame->extended_data, 
               frame->nb_samples
            );
            
            pcm_buf.insert(pcm_buf.end(), conv_buf.begin(), conv_buf.end());

            while (pcm_buf.size() >= 11520)
            {
               v->voiceclient->send_audio_raw(
                  (uint16_t*)pcm_buf.data(), 
                  11520
               );
               pcm_buf.erase(pcm_buf.begin(), pcm_buf.begin() + 11520);
            }
            
         }

         av_packet_unref(pkt);
         
      }

      av_frame_free(&frame);
      av_packet_free(&pkt);
      swr_free(&swr);
      avcodec_free_context(&dec_ctx);
      avformat_close_input(&fmt);

      musicHandler.setIsPlaying(false);
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
   }
   
}