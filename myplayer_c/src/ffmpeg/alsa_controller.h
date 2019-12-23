#ifndef __ALSA_CONTROLLER_H_
#define __ALSA_CONTROLLER_H_

#include <alsa/asoundlib.h>

struct alsa_controller
{
    struct snd_pcm_t *pcm_handler;
    char pcm_device[64];
    int (*prepare)();
    int (*init)(const char *device);
    int (*pause)();
    int (*resume)();
    int (*abort)();
    int (*write_stream)();
    int (*clear)();
    int (*close)();
    
};

extern struct alsa_controller alsa_controller_instance;
#endif
