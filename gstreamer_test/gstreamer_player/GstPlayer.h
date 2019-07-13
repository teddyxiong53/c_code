#ifndef AACE_AUDIO_GSTREAMER_AUDIOMIXER_H_
#define AACE_AUDIO_GSTREAMER_AUDIOMIXER_H_

#include <memory>
#include <gst/gst.h>

#include "Context.h"


class GstPlayer : public Context
{
public:
	static std::unique_ptr<GstPlayer> create(
		const std::string &name,
		const std::string &device);

	void setURI(const std::string &uri);
	void setVolume(double volume);
	void setMute(bool mute);

	// AppSrc interface
	void setStreamCapabilities(const std::string &caps);
	ssize_t write(const char *data, const size_t size);
	void notifyEndOfStream();

	// Event handlers for GStreamer
	void aboutToFinish();
	void sourceSetup(GstElement *source);
	void onNeedData(ssize_t length);
	void onEnoughData();

private:
	GstPlayer(
		const std::string &name,
		const std::string &device);

	bool init();

	GstElement *m_bin = NULL;
	GstElement *m_volume = NULL;
	GstElement *m_sink = NULL;

	const std::string m_name;
	const std::string m_device;

	std::string m_caps;
};


#endif //AACE_AUDIO_GSTREAMER_AUDIOMIXER_H_