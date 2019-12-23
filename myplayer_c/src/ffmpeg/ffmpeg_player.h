#ifndef _FFMPEG_PLAYER_H_
#define _FFMPEG_PLAYER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <pthread.h>
#include "alsa_controller.h"

struct myplayer;
struct ffmpeg_player {
    struct myplayer *player;
    struct AVFormatContext *format_ctx;
    struct AVCodecContext *codec_ctx;
    struct AVCodecParameters *codec_param;
    char *pcm_buffer;
    char *frame_buffer;
    pthread_mutex_t decode_mutex;
    pthread_cond_t decode_cond;
    pthread_mutex_t play_mutext;
    pthread_cond_t play_cond;
    pthread_t decode_thread;
    pthread_t play_thread;

    struct alsa_controller *alsa_ctrl;
    // void (*alsa_controller_init)();
    struct pcm_buf_pool *pcm_bufs;
    
};

#ifdef __cplusplus
}
#endif

#endif
