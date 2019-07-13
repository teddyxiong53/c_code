

#ifndef AACE_AUDIO_GSTREAMER_GSTRECORDER_H_
#define AACE_AUDIO_GSTREAMER_GSTRECORDER_H_

#include <memory>
#include <gst/gst.h>

#include "Context.h"


class GstRecorder : public Context
{
public:
	static std::unique_ptr<GstRecorder> create(
		const std::string &name,
		const std::string &device);

	GstRecorder(
		const std::string &name,
		const std::string &device);

	// Event handlers for GStreamer
	GstFlowReturn onNewSample();

private:
	bool init();

	GstElement *m_source = NULL;
	GstElement *m_sink = NULL;

	const std::string m_name;
	const std::string m_device;
};



#endif //AACE_AUDIO_GSTREAMER_GSTRECORDER_H_