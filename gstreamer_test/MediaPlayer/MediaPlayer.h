#ifndef _DOSS_MEDIA_PLAYER_H_
#define _DOSS_MEDIA_PLAYER_H_

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/base/gstbasesink.h>
#include <pthread.h>


struct AudioPipeline {
	GstAppSrc *appsrc;
	GstElement* decoder;
	GstElement* converter;
	GstElement* volume;
	GstElement* resample;
	GstElement* caps;
	GstElement* audioSink;
	GstElement* pipeline;
};


struct MediaPlayer {
	AudioPipeline m_pipeline;
    GMainLoop* m_mainLoop;
    pthread_t m_playThread;
    guint m_busWatchId;
};

long MediaPlayerInit();


long MediaPlayerDeinit();
MediaPlayer* MediaPlayerCreate(const char *type);


#endif