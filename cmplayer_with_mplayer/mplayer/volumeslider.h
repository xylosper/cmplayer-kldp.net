#ifndef MPLAYERVOLUMESLIDER_H
#define MPLAYERVOLUMESLIDER_H

#include "cslider.h"

namespace MPlayer {

class AudioOutput;

class VolumeSlider : public CSlider {
	Q_OBJECT
public:
	VolumeSlider(AudioOutput *audio, QWidget *parent = 0);
};

}

#endif