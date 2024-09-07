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

#include "openauto/Configuration/Configuration.hpp"
#include "OpenautoLog.hpp"

namespace openauto
{
namespace configuration
{

const std::string Configuration::cConfigFileName = "openauto.ini";

const std::string Configuration::cGeneralShowClockKey = "General.ShowClock";
const std::string Configuration::cGeneralHandednessOfTrafficTypeKey = "General.HandednessOfTrafficType";

const std::string Configuration::cVideoFPSKey = "Video.FPS";
const std::string Configuration::cVideoResolutionKey = "Video.Resolution";
const std::string Configuration::cVideoScreenDPIKey = "Video.ScreenDPI";
const std::string Configuration::cVideoMarginWidth = "Video.MarginWidth";
const std::string Configuration::cVideoMarginHeight = "Video.MarginHeight";

const std::string Configuration::cInputEnableTouchscreenKey = "Input.EnableTouchscreen";

const std::string Configuration::cWifiSSID = "WiFi.SSID";
const std::string Configuration::cWifiPskey = "WiFi.Password";
const std::string Configuration::cLastBluetoothPair = "WiFi.LastBluetoothPair";

Configuration::Configuration()
{
    this->load();
}

void Configuration::load()
{
    boost::property_tree::ptree iniConfig;

    try
    {
        boost::property_tree::ini_parser::read_ini(cConfigFileName, iniConfig);

        handednessOfTrafficType_ = static_cast<HandednessOfTrafficType>(iniConfig.get<uint32_t>(cGeneralHandednessOfTrafficTypeKey,
                                                                                                static_cast<uint32_t>(HandednessOfTrafficType::LEFT_HAND_DRIVE)));
        showClock_ = iniConfig.get<bool>(cGeneralShowClockKey, true);

        videoFPS_ = static_cast<aasdk::proto::enums::VideoFPS::Enum>(iniConfig.get<uint32_t>(cVideoFPSKey,
                                                                                             aasdk::proto::enums::VideoFPS::_60));

        videoResolution_ = static_cast<aasdk::proto::enums::VideoResolution::Enum>(iniConfig.get<uint32_t>(cVideoResolutionKey,
                                                                                                           aasdk::proto::enums::VideoResolution::_480p));
        screenDPI_ = iniConfig.get<size_t>(cVideoScreenDPIKey, 140);

        videoMargins_ = QRect(0, 0, iniConfig.get<int32_t>(cVideoMarginWidth, 0), iniConfig.get<int32_t>(cVideoMarginHeight, 0));

        enableTouchscreen_ = iniConfig.get<bool>(cInputEnableTouchscreenKey, false);

        wifiSSID_ = iniConfig.get<std::string>(cWifiSSID, "");
        wifiPassword_ = iniConfig.get<std::string>(cWifiPskey, "");
        lastBluetoothPair_ = iniConfig.get<std::string>(cLastBluetoothPair, "");
    }
    catch(const boost::property_tree::ini_parser_error& e)
    {
        OPENAUTO_LOG(warning) << "[Configuration] failed to read configuration file: " << cConfigFileName
                            << ", error: " << e.what()
                            << ". Using default configuration.";
        this->reset();
    }
}

void Configuration::reset()
{
    handednessOfTrafficType_ = HandednessOfTrafficType::RIGHT_HAND_DRIVE;
    showClock_ = true;
    videoFPS_ = aasdk::proto::enums::VideoFPS::_60;
    videoResolution_ = aasdk::proto::enums::VideoResolution::_480p;
    screenDPI_ = 140;
    videoMargins_ = QRect(0, 0, 0, 0);
    enableTouchscreen_ = true;
}

void Configuration::save()
{
    boost::property_tree::ptree iniConfig;
    iniConfig.put<uint32_t>(cGeneralHandednessOfTrafficTypeKey, static_cast<uint32_t>(handednessOfTrafficType_));
    iniConfig.put<bool>(cGeneralShowClockKey, showClock_);

    iniConfig.put<uint32_t>(cVideoFPSKey, static_cast<uint32_t>(videoFPS_));
    iniConfig.put<uint32_t>(cVideoResolutionKey, static_cast<uint32_t>(videoResolution_));
    iniConfig.put<size_t>(cVideoScreenDPIKey, screenDPI_);
    iniConfig.put<uint32_t>(cVideoMarginWidth, videoMargins_.width());
    iniConfig.put<uint32_t>(cVideoMarginHeight, videoMargins_.height());

    iniConfig.put<bool>(cInputEnableTouchscreenKey, enableTouchscreen_);

    iniConfig.put<std::string>(cWifiSSID, wifiSSID_);
    iniConfig.put<std::string>(cWifiPskey, wifiPassword_);
    iniConfig.put<std::string>(cLastBluetoothPair, lastBluetoothPair_);
    boost::property_tree::ini_parser::write_ini(cConfigFileName, iniConfig);
}

HandednessOfTrafficType Configuration::getHandednessOfTrafficType() const
{
    return handednessOfTrafficType_;
}

bool Configuration::showClock() const
{
    return showClock_;
}

aasdk::proto::enums::VideoFPS::Enum Configuration::getVideoFPS() const
{
    return videoFPS_;
}

aasdk::proto::enums::VideoResolution::Enum Configuration::getVideoResolution() const
{
    return videoResolution_;
}

size_t Configuration::getScreenDPI() const
{
    return screenDPI_;
}

QRect Configuration::getVideoMargins() const
{
    return videoMargins_;
}

bool Configuration::getTouchscreenEnabled() const
{
    return enableTouchscreen_;
}

std::string Configuration::getWifiSSID()
{
    return wifiSSID_;
}

std::string Configuration::getWifiPassword()
{
    return wifiPassword_;
}

std::string Configuration::getLastBluetoothPair()
{
    return lastBluetoothPair_;
}

void Configuration::setLastBluetoothPair(std::string value)
{
    lastBluetoothPair_ = value;
}

}
}
