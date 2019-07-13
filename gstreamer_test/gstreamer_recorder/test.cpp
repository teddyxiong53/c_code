#include "GstAudioCapture.h"
#include "mylog.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "GstUtils.h"
int g_fd;

ssize_t saveData(const int16_t *data, const size_t size)
{
    //mylogd("size:%d", size);
    return write(g_fd, (int8_t *)data, size * 2);
}

int main()
{
    GstUtils::initializeGStreamer();
    system("rm -rf 1.pcm");
    g_fd = open("1.pcm", O_RDWR | O_CREAT, 0777);
    if (g_fd < 0)
    {
        myloge("open file fail");
        return -1;
    }

    std::shared_ptr<GstAudioCapture> audioCapture = GstAudioCapture::create("gst-recorder-test", "default");
    audioCapture->startAudioInput(saveData);
    sleep(3);
    audioCapture->stopAudioInput();
    close(g_fd);
}
