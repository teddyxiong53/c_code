#include <stdio.h>
#include "myplayer.h"

struct myplayer *g_player = &ffmpeg_player_instance;

int main()
{
    g_player->init(g_player);
    g_player->set_uri("./1.mp3");
    g_player->start();
    sleep(3);
    g_player->pause();
    sleep(2);
    g_player->resume();

}

