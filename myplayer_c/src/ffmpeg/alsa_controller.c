#include "alsa_controller.h"

int alsa_controller_prepare()
{
    if(alsa_controller_instance.pcm_handler != NULL) {
        snd_pcm_prepare(alsa_controller_instance.pcm_handler);
    }
    return 0;
}

int alsa_controller_open()
{
    int ret = 0;
    ret = snd_pcm_open(&alsa_controller_instance.pcm_handler,
        alsa_controller_instance.pcm_device, SND_PCM_STREAM_PLAYBACK, 0);
    return ret;
}

int alsa_controller_set_params(unsigned int rate,unsigned int  channels)()
{
    
    return 0;
}
int alsa_controller_init(const char *device, unsigned int rate, unsigned int channels)
{
    int ret = -1;
    strcpy(alsa_controller_instance.pcm_device, device);
    if(alsa_controller_open() < 0) {
        myloge("open device %s fail\n", device);
        return -1;
    }
    ret = alsa_controller_set_params(rate, channels);
    if(ret) {
        myloge("set param fail\n");
        alsa_controller_close();
    }
    return 0;
}


struct alsa_controller alsa_controller_instance = {
    .pcm_handler = 0,
    .prepare = alsa_controller_prepare,
    .init = alsa_controller_init,

};
