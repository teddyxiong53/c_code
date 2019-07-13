#include "GstMediaPlayer.h"
#include <unistd.h>
#include "GstUtils.h"

int main()
{
    GstUtils::initializeGStreamer();
    std::shared_ptr<GstMediaPlayer> mediaPlayer = nullptr;
    mediaPlayer = GstMediaPlayer::create(GstMediaPlayer::StreamType::MP3_FILE, "test-gst", "default");
    mediaPlayer->prepare("file:///home/teddy/work/test/gstreamer/test/MediaPlayer/1.mp3");
    mediaPlayer->play();
    sleep(3);
    mediaPlayer->pause();
    return 0;
}
