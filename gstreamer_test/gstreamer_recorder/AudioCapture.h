
#include <functional>

#ifndef AACE_AUDIO_AUDIOCAPTURE_H
#define AACE_AUDIO_AUDIOCAPTURE_H


class AudioCapture {
protected:
	AudioCapture() = default;

public:

	virtual bool startAudioInput(const std::function<ssize_t(const int16_t*, const size_t)> &listener) = 0;

	virtual bool stopAudioInput() = 0;
};



#endif // AACE_AUDIO_AUDIOCAPTURE_H