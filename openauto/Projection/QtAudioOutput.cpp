/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QThread>
#include "openauto/Projection/QtAudioOutput.hpp"
#include "OpenautoLog.hpp"


namespace openauto
{
namespace projection
{

QtAudioOutput::QtAudioOutput(uint32_t channelCount, uint32_t sampleSize, uint32_t sampleRate)
    : playbackStarted_(false)
{
    audioFormat_.setChannelCount(channelCount);
    audioFormat_.setSampleRate(sampleRate);
    audioFormat_.setSampleSize(sampleSize);
    audioFormat_.setCodec("audio/pcm");
    audioFormat_.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat_.setSampleType(QAudioFormat::SignedInt);

    QThread * th = QApplication::instance()->thread();
    this->moveToThread(th);
    th->setPriority(QThread::TimeCriticalPriority);

    connect(this, &QtAudioOutput::startPlayback, this, &QtAudioOutput::onStartPlayback);
    connect(this, &QtAudioOutput::suspendPlayback, this, &QtAudioOutput::onSuspendPlayback);
    connect(this, &QtAudioOutput::stopPlayback, this, &QtAudioOutput::onStopPlayback);

    QMetaObject::invokeMethod(this, "createAudioOutput", Qt::BlockingQueuedConnection);
}

void QtAudioOutput::createAudioOutput()
{
    OPENAUTO_LOG(debug) << "[QtAudioOutput] create.";
    audioOutput_ = std::make_unique<QAudioOutput>(QAudioDeviceInfo::defaultOutputDevice(), audioFormat_);
    audioOutput_->setBufferSize(audioFormat_.bytesForDuration(1000 * 1000));
}

bool QtAudioOutput::open()
{
    return true;
}

void QtAudioOutput::write(aasdk::messenger::Timestamp::ValueType, const aasdk::common::DataConstBuffer& buffer)
{
    auto outputState = audioOutput_->state();
    if (outputState != QAudio::StoppedState && outputState != QAudio::SuspendedState)
    {
        audioBuffer_->write(reinterpret_cast<const char*>(buffer.cdata), buffer.size);
    }
}

void QtAudioOutput::start()
{
    emit startPlayback();
}

void QtAudioOutput::stop()
{
    emit stopPlayback();
}

void QtAudioOutput::suspend()
{
    emit suspendPlayback();
}

uint32_t QtAudioOutput::getSampleSize() const
{
    return audioFormat_.sampleSize();
}

uint32_t QtAudioOutput::getChannelCount() const
{
    return audioFormat_.channelCount();
}

uint32_t QtAudioOutput::getSampleRate() const
{
    return audioFormat_.sampleRate();
}


void QtAudioOutput::onStartPlayback()
{
    if(!playbackStarted_)
    {
        audioBuffer_ = audioOutput_->start();
        playbackStarted_ = true;
    }
    else
    {
        audioOutput_->resume();
    }
}

void QtAudioOutput::onSuspendPlayback()
{
    audioOutput_->suspend();
}

void QtAudioOutput::onStopPlayback()
{
    if(playbackStarted_)
    {
        audioOutput_->stop();
        playbackStarted_ = false;
    }
}

}
}
