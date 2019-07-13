#include "GstMediaPlayer.h"
#include <unistd.h>
#include "GstUtils.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "mylog.h"

#if 1
int main()
{
    GstUtils::initializeGStreamer();
    std::shared_ptr<GstMediaPlayer> mediaPlayer = nullptr;
    mediaPlayer = GstMediaPlayer::create(GstMediaPlayer::StreamType::MP3_FILE, "test-gst", "");
    mediaPlayer->prepare("file:///home/teddy/work/test/gstreamer/test/MediaPlayer/1.mp3");
    mediaPlayer->play();
    mylogd("current volume:%d", mediaPlayer->getVolume());
    mediaPlayer->setVolume(80);
    mylogd("current volume:%d", mediaPlayer->getVolume());
    sleep(3);
    mediaPlayer->pause();
    mylogd("current position:%lld", mediaPlayer->getPosition());
    mylogd("is muted:%d", mediaPlayer->isMuted());
    mediaPlayer->resume();
    mediaPlayer->setVolume(50);
    mediaPlayer->setPosition(10*1000);
    mylogd("current position:%lld", mediaPlayer->getPosition());
    sleep(3);
    return 0;
}
#else
int main(int argc, char **argv)
{
    //g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING| G_LOG_LEVEL_CRITICAL|G_LOG_LEVEL_DEBUG|G_LOG_LEVEL_INFO, g_log_default_handler, NULL);
    g_warning("This is warning\n");
    g_message("This is message\n");
    g_debug("This is debug\n");
    g_critical("This is critical\n");
    g_log(NULL, G_LOG_LEVEL_INFO , "This is info\n");
    return 0;
}
#endif