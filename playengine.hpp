#ifndef PLAYENGINE_HPP
#define PLAYENGINE_HPP

#include <QtCore/QObject>
#include "global.hpp"
#include "mrl.hpp"

class NativeVideoRenderer;	class AudioController;

class PlayEngine : public QObject {
	Q_OBJECT
public:
	PlayEngine();
	~PlayEngine();
	int position() const;
	void setMrl(const Mrl &mrl);
	MediaState state() const;
	bool isSeekable() const;
	void setSpeed(double speed);
	double speed() const;
	bool hasVideo() const;
	bool hasAudio() const;
	MediaStatus status() const;
	int duration() const;
	bool atEnd() const;
	NativeVideoRenderer *renderer() const;
	AudioController *audio() const;
	Mrl mrl() const;
	void flush();
//	void setMuted(bool muted);
//	void setVolumeAmp(int volume, double amp);
public slots:
	bool play();
	void stop();
	bool pause();
	bool seek(int pos);
	void navigateDVDMenu(int cmd);
signals:
	void finished();
	void tick(int pos);
	void mrlChanged(const Mrl &mrl);
	void stateChanged(MediaState state, MediaState old);
	void seekableChanged(bool seekable);
	void speedChanged(double speed);
	void positionChanged(int pos);
	void hasVideoChanged(bool has);
	void hasAudioChanged(bool has);
	void streamsChanged();
	void durationChanged(int duration);
	void tagsChanged();
	void statusChanged(MediaStatus status);
private slots:
	void handleGstMessage(void *ptr);
	void ticking();
private:
	void setSeekable(bool seekable);
	void setState(MediaState state);
	void setStatus(MediaStatus status);
	void eos();
	void getStreamInfo();
	void queryDuration();
private:
	void finish();
	struct Data;
	Data *d;
};

#endif // PLAYENGINE_HPP
