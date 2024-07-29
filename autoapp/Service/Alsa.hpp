#pragma once

#include <QObject>
#include <QThread>
#include <alsa/asoundlib.h>

namespace autoapp
{
namespace service
{

class Alsa : public QThread
{
    Q_OBJECT


public:
    void adjustVolumeRelative(int);
    void toggleMute();

private:
    struct VolumeRange
    {
        long min;
        long max;
    };

    snd_mixer_t *mixerHandle_;
    snd_mixer_elem_t *mixerElement_;
    VolumeRange volumeRange_;
    int lastVolumePercent_;
    void run();

signals:
    void onVolumeChange(int);

};

}
}
