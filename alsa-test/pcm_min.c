#include <alsa/asoundlib.h>
#include "mylog.h"

#define BUF_SIZE 16*1024
unsigned char buffer[BUF_SIZE];

int main(int argc, char const *argv[])
{
    int err;
    unsigned int i;
    snd_pcm_t *handle;
    snd_pcm_sframes_t frames;

    //产生一段随机数据。
    for(i=0; i<sizeof(buffer); i++) {
        buffer[i] = random() % 0xff;
    }
    //打开播放handle
    err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if(err) {
        myloge("open default fail");
        return -1;
    }
    //设置播放参数。
    err = snd_pcm_set_params(
        handle,
        SND_PCM_FORMAT_U8,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1,
        48000,
        1,
        500000
    );
    if(err) {
        myloge("set param fail");
        return -1;
    }
    //播放
    for(i=0; i<16; i++) {
        frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
        if(frames < 0) {
            myloge("writei fail");
            frames = snd_pcm_recover(handle, frames, 0);
        }
        if(frames < 0) {
            //recover了还是有错误。
            myloge("recover fail, reason:%s", snd_strerror(frames));
            break;
        }
        if(frames > 0 && frames<sizeof(buffer)) {
            mylogd("write not complete: (%d/%d)", frames, BUF_SIZE);
        }
    }
    //关闭设备
    snd_pcm_close(handle);
    return 0;
}

