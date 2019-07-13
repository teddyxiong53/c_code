#include <thread>

#include <gst/app/gstappsink.h>

#include "GstAudioCapture.h"
static const std::string TAG("aace.audio.GstAudioCapture");

std::shared_ptr<GstAudioCapture> GstAudioCapture::create(
	const std::string &name,
	const std::string &device)
{
	std::shared_ptr<GstAudioCapture> speechRecognizer
		= std::make_shared<GstAudioCapture>(name, device);
	if (!speechRecognizer->init())
		return nullptr;

	return speechRecognizer;
}

GstAudioCapture::GstAudioCapture(const std::string &name, const std::string &device) :
	m_name{name},
	m_device{device} {}

bool GstAudioCapture::init()
{
	m_recorder = GstRecorder::create(m_name, m_device);
	if (!m_recorder)
		return false;

	m_recorder->setListener(this);

	return true;
}

void GstAudioCapture::onStreamData(const int16_t *data, const size_t length)
{
	m_listener(data, length);
}

bool GstAudioCapture::startAudioInput(const std::function<ssize_t(const int16_t*, const size_t)> &listener)
{
	m_listener = listener;
	AACE_DEBUG(LX(TAG, "play"));
	m_recorder->play();
	return true;
}

bool GstAudioCapture::stopAudioInput()
{
	AACE_DEBUG(LX(TAG, "stop"));
	m_recorder->stop();
	return true;
}