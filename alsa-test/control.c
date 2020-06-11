#include <alsa/asoundlib.h>
#include "mylog.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    snd_ctl_t *handle;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_rawmidi_info_t *rawmidiinfo;
    char str[128];

    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);
    snd_rawmidi_info_alloca(&rawmidiinfo);

    int idx = -1;
    int err = 0;
    while(1) {
        err = snd_card_next(&idx);//本质是操作pcmC0p0这样的节点。
        if(err < 0) {
            myloge("card next error:%s", snd_strerror(err));
            break;
        }
        if(idx < 0) {//靠这里正常退出循环。
            mylogd("idx err:%d", idx);
            break;
        }
        sprintf(str, "hw:CARD=%i", idx);
        err = snd_ctl_open(&handle, str, 0);
        if(err < 0) {
            myloge("open %s fail, reason:%s", str, snd_strerror(err));
            continue;
        }
        printf("soundcard #%i:\n", idx+1);
        printf("card: %d\n", snd_ctl_card_info_get_card(info));

    }
    return 0;
}
