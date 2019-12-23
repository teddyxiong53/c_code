#include "myplayer.h"
#include "ffmpeg_player.h"

#define PCM_BUFFER_SIZE 48000
#define RESAMPLE_OUTPUT_BUFFER_SIZE 192000

void ffmpeg_player_set_uri( char *uri)
{
    strncpy(player->uri, uri, URI_LEN-1);
}

static void* play_thread_proc(void *arg)
{


}
int interrupt_callback(void *ctx)
{

}

void free_format_context(struct ffmpeg_player *priv)
{
    if(priv->format_ctx) {
        avformat_close_input(&priv->format_ctx);
        avformat_free_context(priv->format_ctx);
        priv->format_ctx = NULL;
    }
}

void *decode_thread_proc(void *arg)
{
    struct myplayer *player = (struct myplayer *)arg;
    struct ffmpeg_player *priv = player->priv;
    while(1) {
        pthread_mutex_lock(&priv->decode_mutex);
        while(strlen(player->uri) == 0) {
            pthread_cond_wait(&priv->decode_cond, &priv->decode_mutex);
        }
        priv->alsa_ctrl->prepare();
        int ret = 0;
        int count = 0;
        int flag = 0;
        while(count++ < 5) {
            priv->format_ctx = avforamt_alloc_context();
            priv->format_ctx->interrupt_callback.callback = interrupt_callback;
            priv->format_ctx->interrupt_callback.opaque = priv;
            ret = avformat_open_input(&priv->format_ctx, player->uri,NULL, NULL );
            if(ret == 0) {
                flag = 1;
                break;
            } else {
                myloge("try open input fail");
                free_format_context(priv);
            }
        }
        if(!flag) {
            //说明打开这个uri一直失败，那就不播了。
            memset(player->uri, 0, URI_LEN);
            free_format_context(priv);
            pthread_mutex_unlock(&priv->decode_mutex);
            continue;
        }
    }
}

void ffmpeg_player_log_callback(void *ptr, int level, const char *fmt, va_list vl)
{
    vprintf(fmt, vl);
}
int ffmpeg_player_init(struct myplayer *player)
{
    struct ffmpeg_player *priv = player->priv;
    priv->player = player;
    av_log_set_callback(ffmpeg_player_log_callback);
    av_log_set_level(AV_LOG_INFO);
    av_register_all();
    avcodec_register_all();
    avformat_network_init();
    priv->pcm_buffer = av_malloc(PCM_BUFFER_SIZE);
    priv->frame_buffer = av_malloc(RESAMPLE_OUTPUT_BUFFER_SIZE);
    priv->alsa_ctrl = &alsa_controller_instance;
    priv->alsa_ctrl->init("default");
    priv->pcm_bufs = malloc(sizeof(struct pcm_buf_pool));
    priv->pcm_bufs->init();
    pthread_mutex_init(&priv->decode_mutex, NULL);
    pthread_mutex_init(&priv->play_mutex,NULL);
    pthread_cond_init(&priv->decode_cond, NULL);
    pthread_cond_init(&priv->play_cond, NULL);
    pthread_create(&priv->play_thread, NULL, play_thread_proc, player);
    pthread_create(&priv->decode_thread, NULL, decode_thread_proc, player);
}

void ffmpeg_player_deinit()
{
    mylogd("");
}

struct ffmpeg_player player_priv ;

struct myplayer ffmpeg_player_instance = {
    .set_uri = ffmpeg_player_set_uri,
    .init = ffmpeg_player_init,
    .deinit = ffmpeg_player_deinit,

};
