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
#include "aasdk_proto/ButtonCodeEnum.pb.h"
#include "BluetootAdapterType.hpp"
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
    typedef std::vector<aasdk::proto::enums::ButtonCode::Enum> ButtonCodes;

    Configuration();

    void load();
    void reset();
    void save();

    void setHandednessOfTrafficType(HandednessOfTrafficType value);
    HandednessOfTrafficType getHandednessOfTrafficType() const;
    void showClock(bool value);
    bool showClock() const;

    aasdk::proto::enums::VideoFPS::Enum getVideoFPS() const;
    void setVideoFPS(aasdk::proto::enums::VideoFPS::Enum value);
    aasdk::proto::enums::VideoResolution::Enum getVideoResolution() const;
    void setVideoResolution(aasdk::proto::enums::VideoResolution::Enum value);
    size_t getScreenDPI() const;
    void setScreenDPI(size_t value);
    void setOMXLayerIndex(int32_t value);
    int32_t getOMXLayerIndex() const;
    void setVideoMargins(QRect value);
    QRect getVideoMargins() const;
    void setWhitescreenWorkaround(bool value);
    bool getWhitescreenWorkaround() const;

    bool getTouchscreenEnabled() const;
    void setTouchscreenEnabled(bool value);
    ButtonCodes getButtonCodes() const;
    void setButtonCodes(const ButtonCodes& value);

    BluetoothAdapterType getBluetoothAdapterType() const;
    void setBluetoothAdapterType(BluetoothAdapterType value);
    std::string getBluetoothRemoteAdapterAddress() const;
    void setBluetoothRemoteAdapterAddress(const std::string& value);

    bool musicAudioChannelEnabled() const;
    void setMusicAudioChannelEnabled(bool value);
    bool speechAudioChannelEnabled() const;
    void setSpeechAudioChannelEnabled(bool value);
    AudioOutputBackendType getAudioOutputBackendType() const;
    void setAudioOutputBackendType(AudioOutputBackendType value);

    std::string getWifiSSID();
    void setWifiSSID(std::string value);
    std::string getWifiPassword();
    void setWifiPassword(std::string value);
    std::string getWifiMAC();
    void setWifiMAC(std::string value);
    bool getAutoconnectBluetooth();
    void setAutoconnectBluetooth(bool value);
    std::string getLastBluetoothPair();
    void setLastBluetoothPair(std::string value);

private:
    void readButtonCodes(boost::property_tree::ptree& iniConfig);
    void insertButtonCode(boost::property_tree::ptree& iniConfig, const std::string& buttonCodeKey, aasdk::proto::enums::ButtonCode::Enum buttonCode);
    void writeButtonCodes(boost::property_tree::ptree& iniConfig);

    HandednessOfTrafficType handednessOfTrafficType_;
    bool showClock_;
    aasdk::proto::enums::VideoFPS::Enum videoFPS_;
    aasdk::proto::enums::VideoResolution::Enum videoResolution_;
    size_t screenDPI_;
    int32_t omxLayerIndex_;
    QRect videoMargins_;
    bool whitescreenWorkaround_;
    bool enableTouchscreen_;
    ButtonCodes buttonCodes_;
    BluetoothAdapterType bluetoothAdapterType_;
    std::string bluetoothRemoteAdapterAddress_;
    bool musicAudioChannelEnabled_;
    bool speechAudiochannelEnabled_;
    AudioOutputBackendType audioOutputBackendType_;
    std::string wifiSSID_;
    std::string wifiPassword_;
    std::string wifiMAC_;
    bool autoconnectBluetooth_;
    std::string lastBluetoothPair_;

    static const std::string cConfigFileName;

    static const std::string cGeneralShowClockKey;
    static const std::string cGeneralHandednessOfTrafficTypeKey;

    static const std::string cVideoFPSKey;
    static const std::string cVideoResolutionKey;
    static const std::string cVideoScreenDPIKey;
    static const std::string cVideoOMXLayerIndexKey;
    static const std::string cVideoMarginWidth;
    static const std::string cVideoMarginHeight;
    static const std::string cVideoWhitescreenWorkaround;

    static const std::string cAudioMusicAudioChannelEnabled;
    static const std::string cAudioSpeechAudioChannelEnabled;
    static const std::string cAudioOutputBackendType;

    static const std::string cBluetoothAdapterTypeKey;
    static const std::string cBluetoothRemoteAdapterAddressKey;

    static const std::string cInputEnableTouchscreenKey;
    static const std::string cInputPlayButtonKey;
    static const std::string cInputPauseButtonKey;
    static const std::string cInputTogglePlayButtonKey;
    static const std::string cInputNextTrackButtonKey;
    static const std::string cInputPreviousTrackButtonKey;
    static const std::string cInputHomeButtonKey;
    static const std::string cInputPhoneButtonKey;
    static const std::string cInputCallEndButtonKey;
    static const std::string cInputVoiceCommandButtonKey;
    static const std::string cInputLeftButtonKey;
    static const std::string cInputRightButtonKey;
    static const std::string cInputUpButtonKey;
    static const std::string cInputDownButtonKey;
    static const std::string cInputScrollWheelButtonKey;
    static const std::string cInputBackButtonKey;
    static const std::string cInputEnterButtonKey;

    static const std::string cWifiSSID;
    static const std::string cWifiPskey;
    static const std::string cWifiMAC;
    static const std::string cAutoconnectBluetooth;
    static const std::string cLastBluetoothPair;
};

}
}
