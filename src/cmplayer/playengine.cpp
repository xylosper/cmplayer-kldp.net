#include "audiocontroller.hpp"
#include "playengine.hpp"
#include "recentinfo.hpp"
#include "videorenderer.hpp"
#include "vlcmedia.hpp"
#include "libvlc.hpp"
#include "pref.hpp"
#include "mrl.hpp"
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

struct PlayEngine::Data {
	int stoppedTime, duration, prevTick, seek;
	bool seekable, hasVideo;
	double speed;
	TrackList audioTracks, titles, chapters;
	libvlc_media_player_t *mp;
	VlcMedia *media;
	MediaState state;
	MediaStatus status;
	QTimer ticker;
};

PlayEngine::PlayEngine(): d(new Data) {
	qRegisterMetaType<MediaState>("MediaState");
	d->mp = LibVlc::mp();
	d->media = 0;
	d->prevTick = 0;
	d->stoppedTime = d->seek = -1;
	d->state = StoppedState;
	d->status = NoMediaStatus;
	d->hasVideo = d->seekable = false;
	d->speed = 1.0;
	d->duration = 0;

	connect(this, SIGNAL(_ticking()), this, SLOT(ticking()));
	connect(this, SIGNAL(_updateDuration(int)), this, SLOT(updateDuration(int)));
	connect(this, SIGNAL(_updateSeekable(bool)), this, SLOT(updateSeekable(bool)));
	connect(this, SIGNAL(_updateState(MediaState)), this, SLOT(updateState(MediaState)));
	connect(this, SIGNAL(_updateTitle(int)), this, SLOT(updateTitle(int)));
}

PlayEngine::~PlayEngine() {
	stop();
	d->ticker.stop();
	delete d->media;
	delete d;
}

void PlayEngine::initialSeek() {
	if (d->seek < 0)
		return;
	seek(d->seek);
	d->seek = -1;
}

void PlayEngine::updateDuration(int duration) {
	emit durationChanged(d->duration = duration);
//	d->audioTracks = parseTrackDesc(libvlc_audio_get_track_description(d->mp));
//	d->titles = parseTrackDesc(libvlc_video_get_title_description(d->mp));
	if (d->titles != d->titles)
	emit titlesChanged(d->titles);
	emit audioTracksChanged(d->audioTracks);
	qDebug() << "update duration" << duration;
	qDebug() << "title" << libvlc_media_player_get_title(d->mp);
	qDebug() << "audio" << libvlc_audio_get_track_count(d->mp);
//	TrackList tracks = parseTrackDesc(libvlc_video_get_audio_get_track_description(d->mp));
//	for (int i=0; i<tracks.size(); ++i)
//		qDebug() << tracks[i].id << tracks[i].name;
}

void PlayEngine::updateSeekable(bool seekable) {
	if (d->seekable != seekable)
		emit seekableChanged(d->seekable = seekable);
}

void PlayEngine::parseEvent(const libvlc_event_t *event) {
	switch (event->type) {
	case libvlc_MediaPlayerSeekableChanged: {
		const bool seekable = libvlc_media_player_is_seekable(LibVlc::mp());
		emit _updateSeekable(seekable);
		break;
	} case libvlc_MediaPlayerTimeChanged: {
		emit _ticking();
		break;
	} case libvlc_MediaPlayerPlaying:
		emit _updateState(PlayingState);
		break;
	case libvlc_MediaPlayerPaused:
		emit _updateState(PausedState);
		break;
	case libvlc_MediaPlayerEndReached:
		emit _updateState(FinishedState);
		break;
	case libvlc_MediaPlayerOpening:
	case libvlc_MediaPlayerBuffering:
	case libvlc_MediaPlayerStopped:
	case libvlc_MediaPlayerEncounteredError:
		emit _updateState(StoppedState);
		break;
	case libvlc_MediaPlayerLengthChanged:
		emit _updateDuration(event->u.media_player_length_changed.new_length);
		break;
	case libvlc_MediaPlayerTitleChanged:
		emit _updateTitle(event->u.media_player_title_changed.new_title);
		break;
	default:
		break;
	}
}

void PlayEngine::setMrl(const Mrl &mrl) {
	if (isPlaying())
		stop();
	if (d->media) {
		if (d->media->mrl() != mrl) {
			delete d->media;
			d->media = 0;
		}
	}
	if (!d->media) {
		d->media = new VlcMedia(mrl);
		emit mrlChanged(d->media->mrl());
	}
}

bool PlayEngine::atEnd() const {
	return d->status == EosStatus;
}

MediaState PlayEngine::state() const {
	return d->state;
}

bool PlayEngine::isSeekable() const {
	return d->seekable;
}

void PlayEngine::updateTitle(int id) {
	qDebug() << "update title to" << id;
}

TrackList PlayEngine::parseTrackDesc(libvlc_track_description_t *desc) {
	TrackList tracks;
	while (desc) {
		Track track;
		track.id = desc->i_id;
		track.name = QString::fromLocal8Bit(desc->psz_name);
		tracks << track;
		desc = desc->p_next;
	}
	libvlc_track_description_release(desc);
	return tracks;
}

void PlayEngine::updateState(MediaState state) {
	if (d->state != state) {
		const MediaState old = d->state;
		d->state = state;
		emit stateChanged(d->state, old);
		if (d->state == FinishedState) {
			emit aboutToFinished();
			if (d->media)
				emit finished(d->media->mrl());
		} else if (d->state == StoppedState) {
			if (d->stoppedTime >= 0 && d->media) {
				emit stopped(d->media->mrl(), d->stoppedTime, duration());
				d->stoppedTime = -1;
			}
		} else if (d->state == PlayingState && old != PausedState) {
			qDebug() << "play started";
			const bool hasVideo = (libvlc_media_player_has_vout(d->mp) > 0);
			if (d->hasVideo != hasVideo)
				emit hasVideoChanged(d->hasVideo = hasVideo);
			d->audioTracks.clear();
			d->titles.clear();
			d->chapters.clear();
			d->audioTracks = parseTrackDesc(libvlc_audio_get_track_description(d->mp));
			d->titles = parseTrackDesc(libvlc_video_get_title_description(d->mp));
			emit audioTracksChanged(d->audioTracks);
			emit titlesChanged(d->titles);
		}
		if ((d->state == PlayingState) && d->seek >= 0)
			QTimer::singleShot(100, this, SLOT(initialSeek()));
	}
}

double PlayEngine::speed() const {
	return d->speed;
}

void PlayEngine::setSpeed(double speed) {
	if (qFuzzyCompare(speed, 1.0))
		speed = 1.0;
	if (!qFuzzyCompare(d->speed, speed)) {
		d->speed = speed;
		if (isPlaying() || isPaused())
			libvlc_media_player_set_rate(d->mp, d->speed);
		emit speedChanged(d->speed);
	}
}

void PlayEngine::setStatus(MediaStatus status) {
	if (d->status != status) {
		emit statusChanged(d->status = status);
	}
}

bool PlayEngine::play() {
	if (isPaused()) {
		libvlc_media_player_set_pause(d->mp, 0);
		updateState(PlayingState);
		return true;
	}
	if (!d->media)
		return false;
	libvlc_media_player_set_media(d->mp, d->media->media());

	d->seek = 1;
	const RecentInfo &recent = RecentInfo::get();
	const int record = recent.stoppedTime(d->media->mrl());
	if (record > 0) {
		if (Pref::get().askWhenRecordFound) {
			const QDateTime date = recent.stoppedDate(d->media->mrl());
			const QString title = tr("Stopped Record Found");
			const QString text = tr("This file was stopped during its playing before.\n"
				"Played Date: %1\nStopped Time: %2\n"
				"Do you want to start from where it's stopped?\n"
				"(You can configure not to ask anymore in the preferecences.)")
				.arg(date.toString(Qt::ISODate)).arg(msecsToString(record, "h:mm:ss"));
			const QMessageBox::StandardButtons b = QMessageBox::Yes | QMessageBox::No;
			if (QMessageBox::question(QApplication::activeWindow(), title, text, b) == QMessageBox::Yes)
				d->seek = record;
		} else
			d->seek = record;
	}
	if (libvlc_media_player_play(d->mp) != 0) {
	    qDebug() << "libvlc exception:" << libvlc_errmsg();
	    return false;
	}
	return true;
}

bool PlayEngine::pause() {
	libvlc_media_player_set_pause(d->mp, 1);
	updateState(PausedState);
	return true;
}

void PlayEngine::stop() {
	if (isPlaying() || isPaused())
		d->stoppedTime = position();
	else
		d->stoppedTime = -1;
	libvlc_media_player_stop(d->mp);
	updateState(StoppedState);
}

bool PlayEngine::seek(int ms) {
	if (d->state == PlayingState || d->state == PausedState) {
		libvlc_media_player_set_time(d->mp, ms);
		ticking();
		return true;
	} else
		return false;
}

int PlayEngine::duration() const {
	return d->duration;
}

int PlayEngine::position() const {
	int ret = 0;
	if (isPlaying() || isPaused())
		ret = libvlc_media_player_get_time(d->mp);
	return ret;
}

void PlayEngine::ticking() {
	int tick = position();
	if (d->prevTick != tick) {
		emit this->tick(d->prevTick = tick);
	}
}

Mrl PlayEngine::mrl() const {
	return d->media ? d->media->mrl() : Mrl();
}

bool PlayEngine::hasVideo() const {
	return d->hasVideo;
}

QList<Track> PlayEngine::audioTracks() const {
	return d->audioTracks;
}

int PlayEngine::currentAudioTrackId() const {
	return libvlc_audio_get_track(d->mp);
}

void PlayEngine::setCurrentAudioTrack(int id) {
	if (libvlc_audio_set_track(d->mp, qMax(0, id)) != 0)
		 qDebug() << "libvlc exception:" << libvlc_errmsg();
}

QString PlayEngine::audioTrackName(int id) const {
	for (int i=0; i<d->audioTracks.size(); ++i) {
		if (id == d->audioTracks[i].id)
			return d->audioTracks[i].name;
	}
	return QString();
}

int PlayEngine::currentTitleId() const {
	return libvlc_media_player_get_title(d->mp);
}
