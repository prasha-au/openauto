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
#include <QScreen>
#include "aasdk/Channel/AV/MediaAudioServiceChannel.hpp"
#include "aasdk/Channel/AV/SystemAudioServiceChannel.hpp"
#include "aasdk/Channel/AV/SpeechAudioServiceChannel.hpp"
#include "openauto/Service/ServiceFactory.hpp"
#include "openauto/Service/VideoService.hpp"
#include "openauto/Service/MediaAudioService.hpp"
#include "openauto/Service/SpeechAudioService.hpp"
#include "openauto/Service/SystemAudioService.hpp"
#include "openauto/Service/AudioInputService.hpp"
#include "openauto/Service/SensorService.hpp"
#include "openauto/Service/BluetoothService.hpp"
#include "openauto/Service/InputService.hpp"
#include "openauto/Projection/QtVideoOutput.hpp"
#include "openauto/Projection/QtAudioOutput.hpp"
#include "openauto/Projection/QtAudioInput.hpp"
#include "openauto/Projection/InputDevice.hpp"
#include "openauto/Projection/LocalBluetoothDevice.hpp"
#include "openauto/Service/IAndroidAutoInterface.hpp"
#include "OpenautoLog.hpp"


namespace openauto
{
namespace service
{
ServiceFactory::ServiceFactory(boost::asio::io_service& ioService, configuration::Configuration::Pointer configuration, QWidget *activeArea, bool nightMode)
    : ioService_(ioService)
    , configuration_(std::move(configuration))
    , activeArea_(activeArea)
    , screenGeometry_(this->mapActiveAreaToGlobal(activeArea_))
    , qtVideoOutput_((QGst::init(nullptr, nullptr), std::make_shared<projection::QtVideoOutput>(configuration_, activeArea_)))
    , nightMode_(nightMode)
{
    OPENAUTO_LOG(info) << "SERVICE FACTORY INITED";

}

ServiceList ServiceFactory::create(aasdk::messenger::IMessenger::Pointer messenger, bool useBluetooth)
{
    ServiceList serviceList;

    projection::IAudioInput::Pointer audioInput(new projection::QtAudioInput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
    serviceList.emplace_back(std::make_shared<AudioInputService>(ioService_, messenger, std::move(audioInput)));
    this->createAudioServices(serviceList, messenger, !useBluetooth);

    std::shared_ptr<SensorService> sensorService = std::make_shared<SensorService>(ioService_, messenger, nightMode_);
    sensorService_ = sensorService;
    serviceList.emplace_back(sensorService);

    serviceList.emplace_back(this->createVideoService(messenger));
    if (useBluetooth) {
        serviceList.emplace_back(this->createBluetoothService(messenger));
    }

    std::shared_ptr<InputService> inputService = this->createInputService(messenger);
    inputService_ = inputService;
    serviceList.emplace_back(inputService);
    return serviceList;
}

IService::Pointer ServiceFactory::createVideoService(aasdk::messenger::IMessenger::Pointer messenger)
{
    auto videoOutput(qtVideoOutput_);

    return std::make_shared<VideoService>(ioService_, messenger, std::move(videoOutput));
}

IService::Pointer ServiceFactory::createBluetoothService(aasdk::messenger::IMessenger::Pointer messenger)
{
    OPENAUTO_LOG(info) << "Creating bluetooth device.";
    projection::IBluetoothDevice::Pointer bluetoothDevice = projection::IBluetoothDevice::Pointer(new projection::LocalBluetoothDevice(), std::bind(&QObject::deleteLater, std::placeholders::_1));
    return std::make_shared<BluetoothService>(ioService_, messenger, std::move(bluetoothDevice));
}


std::shared_ptr<InputService> ServiceFactory::createInputService(aasdk::messenger::IMessenger::Pointer messenger)
{
    QRect videoGeometry;
    switch(configuration_->getVideoResolution())
    {
    case aasdk::proto::enums::VideoResolution::_720p:
        videoGeometry = QRect(0, 0, 1280, 720);
        break;

    case aasdk::proto::enums::VideoResolution::_1080p:
        videoGeometry = QRect(0, 0, 1920, 1080);
        break;

    default:
        videoGeometry = QRect(0, 0, 800, 480);
        break;
    }

    //account for margins being applied to android auto
    videoGeometry.setWidth(videoGeometry.width()-configuration_->getVideoMargins().width());
    videoGeometry.setHeight(videoGeometry.height()-configuration_->getVideoMargins().height());


    QObject* inputObject = activeArea_ == nullptr ? qobject_cast<QObject*>(QApplication::instance()) : qobject_cast<QObject*>(activeArea_);
    inputDevice_ = std::make_shared<projection::InputDevice>(*inputObject, configuration_, std::move(screenGeometry_), std::move(videoGeometry));

    return std::make_shared<InputService>(ioService_, messenger, std::move(projection::IInputDevice::Pointer(inputDevice_)));
}

void ServiceFactory::createAudioServices(ServiceList& serviceList, aasdk::messenger::IMessenger::Pointer messenger, bool initOptionalChannels)
{
    if (initOptionalChannels) {
        auto mediaAudioOutput = projection::IAudioOutput::Pointer(new projection::QtAudioOutput(2, 16, 48000), std::bind(&QObject::deleteLater, std::placeholders::_1));
        serviceList.emplace_back(std::make_shared<MediaAudioService>(ioService_, messenger, std::move(mediaAudioOutput)));

        auto speechAudioOutput = projection::IAudioOutput::Pointer(new projection::QtAudioOutput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
        serviceList.emplace_back(std::make_shared<SpeechAudioService>(ioService_, messenger, std::move(speechAudioOutput)));
    }

    auto systemAudioOutput = projection::IAudioOutput::Pointer(new projection::QtAudioOutput(1, 16, 16000), std::bind(&QObject::deleteLater, std::placeholders::_1));
    serviceList.emplace_back(std::make_shared<SystemAudioService>(ioService_, messenger, std::move(systemAudioOutput)));
}

void ServiceFactory::setNightMode(bool nightMode)
{
    nightMode_ = nightMode;
    if(std::shared_ptr<SensorService> sensorService = sensorService_.lock())
    {
        sensorService->setNightMode(nightMode_);
    }
}

void ServiceFactory::sendButtonPress(aasdk::proto::enums::ButtonCode::Enum buttonCode, projection::WheelDirection wheelDirection, projection::ButtonEventType buttonEventType)
{
    if(std::shared_ptr<InputService> inputService = inputService_.lock())
    {
        inputService->sendButtonPress(buttonCode, wheelDirection, buttonEventType);
    }
}

void ServiceFactory::sendKeyEvent(QKeyEvent* event)
{
    if(inputDevice_ != nullptr)
    {
        inputDevice_->eventFilter(activeArea_, event);
    }
}


QRect ServiceFactory::mapActiveAreaToGlobal(QWidget* activeArea)
{
    if(activeArea == nullptr)
    {
        QScreen* screen = QGuiApplication::primaryScreen();
        return screen == nullptr ? QRect(0, 0, 1, 1) : screen->geometry();
    }

    QRect g = activeArea->geometry();
    QPoint p = activeArea->mapToGlobal(g.topLeft());

    return QRect(p.x(), p.y(), g.width(), g.height());
}


}
}
