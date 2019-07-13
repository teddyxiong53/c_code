
#ifndef AACE_AUDIO_GSTREAMER_GSTAUDIOCAPTURE_H_
#define AACE_AUDIO_GSTREAMER_GSTAUDIOCAPTURE_H_

#include <memory>
#include <gst/gst.h>
#include "GstRecorder.h"
#include "AudioCapture.h"


class GstAudioCapture :
	public AudioCapture,
	public Context::Listener,
	public std::enable_shared_from_this<GstAudioCapture>
{
public:
	static std::shared_ptr<GstAudioCapture> create(
		const std::string &name,
		const std::string &device);

	GstAudioCapture(
		const std::string &name,
		const std::string &device);
	~GstAudioCapture() = default;

	// Context::Listener interface
	void onStreamData(const int16_t *data, const size_t length) override;

	// AudioCapture interface
	bool startAudioInput(const std::function<ssize_t(const int16_t*, const size_t)> &listener) override;
	bool stopAudioInput() override;

private:
	bool init();

	const std::string m_name;
	const std::string m_device;

	std::unique_ptr<GstRecorder> m_recorder;

	std::function<ssize_t(const int16_t*, const size_t)> m_listener;
};



#endif //AACE_AUDIO_GSTREAMER_GSTAUDIOCAPTURE_H_