#include "MediaPlayer.h"
#include "mylog.h"
#include <stdlib.h>

/*
	对外的接口，因为是需要long类型的返回值。
	所以都统一用long类型的返回值。
	内部接口，也用long类型。统一就好。int和long区别不大。
	内部接口，用下划线开头。
 */

gboolean _onBusMessage(GstBus *bus, GstMessage *message, gpointer mediaPlayer)
{
	struct MediaPlayer *player = (struct MediaPlayer *)mediaPlayer;
	switch(GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_EOS://播放结束
			if(GST_MESSAGE_SRC(message) == GST_OBJECT_CAST(player->m_pipeline.pipeline)) {
				//只有符合这个条件才进行处理
				mylogd("play finish");
			}
			break;
		case GST_MESSAGE_ERROR://播放错误
			GError *error;
			gchar *debug;
			gst_message_parse_error(message, &error, &debug);
			myloge("error content:%s, debug content:%s", error->message, debug);
			g_error_free(error);
			g_free(debug);
			break;
		case GST_MESSAGE_STATE_CHANGED://播放状态改变
			if(GST_MESSAGE_SRC(message) == GST_OBJECT_CAST(player->m_pipeline.pipeline)) {
				GstState oldState;
				GstState newState;
				GstState pendingState;
				gst_message_parse_state_changed(message, &oldState, &newState, &pendingState);
				mylogd("old state:%s, new state:%d, pending state:%s", gst_element_state_get_name(oldState),
					gst_element_state_get_name(newState), gst_element_state_get_name(pendingState));
				if(newState == GST_STATE_PAUSED) {
					mylogd("paused");
					if(pendingState == GST_STATE_PLAYING) {

					}
				}
			}
			break;
	}
}
static long _setupPipeline(struct MediaPlayer *player)
{
	long ret = -1;
	//创建音频转化器。
	player->m_pipeline.converter = gst_element_factory_make("audioconvert", "convert");
	if(!player->m_pipeline.converter) {
		myloge("converter");
		return -1;
	}
	// 音量控制
	player->m_pipeline.volume = gst_element_factory_make("volume", "volume");
	if(!player->m_pipeline.volume) {
		myloge("volume");
		return -1;
	}
	// 音频输出的目的地
	player->m_pipeline.audioSink = gst_element_factory_make("autoaudiosink", "audio_sink");
	if(!player->m_pipeline.audioSink) {
		myloge("audioSink");
		return -1;
	}
	// 增加对pcm的播放支持
	GstCaps *caps = gst_caps_new_empty_simple("audio/x-raw");
	if(!caps) {
		myloge("caps");
		return -1;
	}
	player->m_pipeline.resample = gst_element_factory_make("audioresample", "resample");
	if(!player->m_pipeline.resample) {
		mylogd("resample");
		return -1;
	}
	player->m_pipeline.caps = gst_element_factory_make("capsfilter", "caps");
	if(!player->m_pipeline.caps) {
		mylogd("caps");
		return -1;
	}
	//设置
	g_object_set(G_OBJECT(player->m_pipeline.caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	//创建pipeline
	player->m_pipeline.pipeline = gst_pipeline_new("audio-pipeline");
	if(!player->m_pipeline.pipeline) {
		myloge("pipeline");
		return -1;
	}
	// 拿到总线
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(player->m_pipeline.pipeline));
	player->m_busWatchId = gst_bus_add_watch(bus, _onBusMessage, player);
	gst_object_unref(bus);

	// 把各个元件串起来
	// 先把元件添加到pipeline。
	gst_bin_add_many(GST_BIN(player->m_pipeline.pipeline), player->m_pipeline.converter, player->m_pipeline.volume, player->m_pipeline.audioSink, NULL);
	// 把resample和cap也添加到pipeline
	gst_bin_add_many(GST_BIN(player->m_pipeline.pipeline), player->m_pipeline.resample, player->m_pipeline.caps, NULL);
	//串联起来
	gst_element_link_many(player->m_pipeline.converter,
			player->m_pipeline.volume,
			player->m_pipeline.resample,
			player->m_pipeline.caps,
			NULL
		);
	gst_element_link_filtered(player->m_pipeline.caps, player->m_pipeline.audioSink, caps);
	mylogd("setup pipeline ok");
	return 0;

}

static void *_mainLoopWrapper(void *arg)
{
	GMainLoop *loop = (GMainLoop *)arg;
	g_main_loop_run(loop);
}
static long _init(struct MediaPlayer *player)
{
	long ret = -1;
	player->m_mainLoop = g_main_loop_new(NULL, false);
	ret = pthread_create(&player->m_playThread , NULL, _mainLoopWrapper, player->m_mainLoop);
	if(ret < 0) {
		myloge("create pthread fail");
		return -1;
	}
	//初始化pipeline
	ret = _setupPipeline(player);
	if(ret < 0) {
		myloge("_setupPipeline fail");
		return -1;
	}
	return 0;
}

long MediaPlayerInit()
{
	gst_init(NULL, NULL);
}


MediaPlayer* MediaPlayerCreate(const char *type)
{
	struct MediaPlayer *player = (struct MediaPlayer *)malloc(sizeof(struct MediaPlayer));
	if(_init(player) < 0) {
		myloge("_init fail");
		free(player);
		player = NULL;
		return NULL;
	}

	return player;
}

long MediaPlayerDeinit()
{

	return 0;
}


