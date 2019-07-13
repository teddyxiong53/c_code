#ifndef AACE_AUDIO_GSTREAMER_GSTUTILS_H_
#define AACE_AUDIO_GSTREAMER_GSTUTILS_H_

#include <memory>
#include <string>
#include <gst/gst.h>



class GstUtils
{
public:
	static bool initializeGStreamer();

	static GstElement *createElement(
		GstElement *bin, const std::string &factory, const std::string &name);
};


#endif //AACE_AUDIO_GSTREAMER_GSTUTILS_H_