#include <iostream>
#include "./Alsa.hpp"
#include <alsa/asoundlib.h>
#include "OpenautoLog.hpp"

namespace autoapp
{
namespace service
{

Alsa::Alsa(QObject *parent)
    : QThread(parent),
    mixerHandle_(nullptr),
    mixerElement_(nullptr),
    lastVolumePercent_(0)
{
}

void Alsa::run()
{
    static const char *selem_name = "Master";
    static const char *card = "default";

    try {
        if (snd_mixer_open(&mixerHandle_, 0) != 0) { throw -1; }
        if (snd_mixer_attach(mixerHandle_, card) != 0) { throw -2; }
        if (snd_mixer_selem_register(mixerHandle_, NULL, NULL) != 0) { throw -3; }


        if (snd_mixer_load(mixerHandle_) != 0) { throw -4; }

        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);

        mixerElement_ = snd_mixer_find_selem(mixerHandle_, sid);
        if (mixerElement_ == nullptr) { throw -5; }

        if (snd_mixer_selem_get_playback_volume_range(mixerElement_, &volumeRange_.min, &volumeRange_.max) != 0) { throw -6; }

    } catch (int exceptionNum) {
        OPENAUTO_LOG(error) << "[Alsa] Failed to initialize mixer object: " << exceptionNum;
        if (mixerHandle_ != nullptr) {
            snd_mixer_close(mixerHandle_);
        }
        mixerElement_ = nullptr;
        return;
    }

    OPENAUTO_LOG(info) << "[Alsa] Service is ready";

    while (true)
    {
        try
        {
            if (snd_mixer_handle_events(mixerHandle_) < 0) {
                throw -6;
            }

            long outvol;
            if (snd_mixer_selem_get_playback_volume(mixerElement_, SND_MIXER_SCHN_FRONT_LEFT, &outvol) < 0) {
                throw -7;
            }

            int volumePercent = 100 * (outvol - volumeRange_.min) / (volumeRange_.max - volumeRange_.min);

            if (lastVolumePercent_ != volumePercent)
            {
                emit onVolumeChange(volumePercent);
                lastVolumePercent_ = volumePercent;
            }
            QThread::msleep(50);
        }
        catch (int exceptionNum)
        {
            OPENAUTO_LOG(error) << "[Alsa] Failed fetching volume " << exceptionNum;
            QThread::msleep(2000);
        }
    }

}

void Alsa::adjustVolumeRelative(int amount)
{
    if (mixerElement_ == nullptr) {
        OPENAUTO_LOG(error) << "[Alsa] Cannot adjust volume - not initialized";
        return;
    }
    int newVolumePercent = lastVolumePercent_ + amount;
    long newVolume = volumeRange_.min + (volumeRange_.max - volumeRange_.min) * newVolumePercent / 100;
    snd_mixer_selem_set_playback_volume_all(mixerElement_, newVolume);
}

void Alsa::toggleMute()
{
    if (mixerElement_ == nullptr) {
        OPENAUTO_LOG(error) << "[Alsa] Cannot adjust volume - not initialized";
        return;
    }
    int value;
    snd_mixer_selem_get_playback_switch(mixerElement_, SND_MIXER_SCHN_MONO, &value);
    snd_mixer_selem_set_playback_switch_all(mixerElement_, !value);
}

}
}
