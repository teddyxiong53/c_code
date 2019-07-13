
#ifndef AACE_AUDIO_GSTREAMER_GSTMEDIAPLAYER_H_
#define AACE_AUDIO_GSTREAMER_GSTMEDIAPLAYER_H_

#include <memory>
#include <thread>
#include <atomic>
#include <gst/gst.h>


#include "GstPlayer.h"



static constexpr uint8_t DEFAULT_VOLUME = 100;

class GstMediaPlayer :
	public Context::Listener
{
public:
	enum class StreamType {
		MP3_FILE,
		MP3_STREAM,
		RAW_STREAM
	};

	static std::shared_ptr<GstMediaPlayer> create(
		const StreamType &type,
		const std::string &name,
		const std::string &device);

	GstMediaPlayer(
		const StreamType &type,
		const std::string &name,
		const std::string &device);
	~GstMediaPlayer() = default;

	// Context::Listener interface
	void onStreamStart() override;
	void onStreamEnd() override;
	void onStreamError() override;
	void onStateChanged(GstState state) override;
	void onDataRequested() override;

	// MediaPlayer interface
	bool prepare() ;
	bool prepare(const std::string &url) ;
	bool play() ;
	bool stop() ;
	bool pause() ;
	bool resume() ;
	int64_t getPosition() ;
	bool setPosition(int64_t position) ;

	// Speaker interface
	bool setVolume(int8_t volume) ;
	bool adjustVolume(int8_t delta) ;
	bool setMute(bool mute) ;
	int8_t getVolume() ;
	bool isMuted() ;

	bool writeStreamToFile(const std::string &path);
	bool writeStreamToPipeline();

private:
	bool init();
	void setURI(const std::string &uri);
	void applyCurrentVolume();

	const std::string TAG;
	const StreamType m_type;
	const std::string m_name;
	const std::string m_device;

	std::unique_ptr<GstPlayer> m_player;

	std::atomic<uint64_t> m_pendingPosition;
	std::string m_currentURI;
	uint8_t m_currentVolume = DEFAULT_VOLUME;
	bool m_currentMute = false;
	int64_t m_currentPosition = 0;
	std::atomic<bool> m_stopRequested;

	std::string m_tmpFile;

#ifdef USE_GLOOP
	GSource *m_timeoutSource = NULL;
#endif
};

#endif //AACE_AUDIO_GSTREAMER_GSTMEDIAPLAYER_H_