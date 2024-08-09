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

#pragma once

#include <string>
#include <QRect>
#include "aasdk_proto/VideoFPSEnum.pb.h"
#include "aasdk_proto/VideoResolutionEnum.pb.h"
#include "HandednessOfTrafficType.hpp"
#include "AudioOutputBackendType.hpp"
#include <boost/property_tree/ini_parser.hpp>


namespace openauto
{
namespace configuration
{

class Configuration
{
public:
    typedef std::shared_ptr<Configuration> Pointer;

    Configuration();

    void load();
    void reset();
    void save();

    void setHandednessOfTrafficType(HandednessOfTrafficType value);
    HandednessOfTrafficType getHandednessOfTrafficType() const;
    bool showClock() const;

    aasdk::proto::enums::VideoFPS::Enum getVideoFPS() const;
    aasdk::proto::enums::VideoResolution::Enum getVideoResolution() const;
    size_t getScreenDPI() const;
    QRect getVideoMargins() const;

    bool getTouchscreenEnabled() const;

    std::string getWifiSSID();
    std::string getWifiPassword();
    std::string getLastBluetoothPair();
    void setLastBluetoothPair(std::string value);

private:
    HandednessOfTrafficType handednessOfTrafficType_;
    bool showClock_;
    aasdk::proto::enums::VideoFPS::Enum videoFPS_;
    aasdk::proto::enums::VideoResolution::Enum videoResolution_;
    size_t screenDPI_;
    QRect videoMargins_;
    bool enableTouchscreen_;
    std::string wifiSSID_;
    std::string wifiPassword_;
    std::string lastBluetoothPair_;

    static const std::string cConfigFileName;

    static const std::string cGeneralShowClockKey;
    static const std::string cGeneralHandednessOfTrafficTypeKey;

    static const std::string cVideoFPSKey;
    static const std::string cVideoResolutionKey;
    static const std::string cVideoScreenDPIKey;
    static const std::string cVideoMarginWidth;
    static const std::string cVideoMarginHeight;

    static const std::string cInputEnableTouchscreenKey;

    static const std::string cWifiSSID;
    static const std::string cWifiPskey;
    static const std::string cLastBluetoothPair;
};

}
}
