
# OpenAuto

### Community
[![Join the chat at https://gitter.im/publiclab/publiclab](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/openauto_androidauto/Lobby)

### Description
OpenAuto is an AndroidAuto(tm) headunit emulator based on aasdk library and Qt libraries. Main goal is to run this application on the RaspberryPI 3 board computer smoothly.

[See demo video](https://www.youtube.com/watch?v=k9tKRqIkQs8)

### Build Guide
#### Local build instructions for Raspberry Pi

Having <a href="https://github.com/openDsh/aasdk">aasdk</a> built and install first is a prequisite for this task. Please complete the necessary aasdk steps before proceeding here.

```sudo apt-get update
sudo apt-get -y install cmake build-essential git

sudo apt-get install libboost-all-dev libusb-1.0.0-dev libssl-dev cmake libprotobuf-dev protobuf-c-compiler protobuf-compiler libqt5multimedia5 libqt5multimedia5-plugins libqt5multimediawidgets5 qtmultimedia5-dev libqt5bluetooth5 libqt5bluetooth5-bin qtconnectivity5-dev pulseaudio

git clone https://github.com/OpenDsh/openauto

mkdir openauto_build; cd openauto_build

cmake -DCMAKE_BUILD_TYPE=Release -DRPI3_BUILD=TRUE -DAASDK_INCLUDE_DIRS="../aasdk/include" -DAASDK_LIBRARIES="../aasdk/lib/libaasdk.so" -DAASDK_PROTO_INCLUDE_DIRS="../aasdk" -DAASDK_PROTO_LIBRARIES="../aasdk/lib/libaasdk_proto.so" ../openauto

make -j2
sudo make install
```

The executable binary can then be found at ~/openauto/bin/autoapp

### Supported functionalities
 - 480p, 720p and 1080p with 30 or 60 FPS
 - RaspberryPI 3 hardware acceleration support to decode video stream (up to 1080p@60!)
 - Audio playback from all audio channels (Media, System and Speech)
 - Audio input for voice commands
 - Touchscreen and buttons input
 - Bluetooth
 - Automatic launch after device hotplug
 - Automatic detection of connected Android devices
 - Wireless (WiFi) mode via head unit server (must be enabled in hidden developer settings)
 - User-friendly settings

### Supported platforms

 - Linux
 - RaspberryPI 3
 - Windows

### License
GNU GPLv3

Copyrights (c) 2018 f1x.studio (Michal Szwaj)

*AndroidAuto is registered trademark of Google Inc.*

### Used software
 - [aasdk](https://github.com/f1xpl/aasdk)
 - [Boost libraries](http://www.boost.org/)
 - [Qt libraries](https://www.qt.io/)
 - [CMake](https://cmake.org/)
 - Broadcom ilclient from RaspberryPI 3 firmware
 - OpenMAX IL API

### Remarks
**This software is not certified by Google Inc. It is created for R&D purposes and may not work as expected by the original authors. Do not use while driving. You use this software at your own risk.**



## New stuff...
Commands
```bash
mkdir build && cd build && cmake ../
cd /workspaces/openauto && ./bin/autoapp
```

Stripped out:
- stripped rtaudio
- stripped omx video
- rename GSTVideoOutput to QtVideoOutput


## Pipe Pulseaudio to Windows...
https://github.com/microsoft/WSL/issues/5816#issuecomment-682242686
https://stackoverflow.com/questions/52890474/how-to-get-docker-audio-and-input-with-windows-or-mac-host

```powershell
cd c:\pulse
.\pulseaudio.exe -F .\config.pa
```


## Pulse Fix
- Add the following to the `/etc/pulse/default.conf`. It may help....
```
; default-fragments = 4
default-fragment-size-msec = 15

; enable-deferred-volume = yes
; deferred-volume-safety-margin-usec = 8000
; deferred-volume-extra-delay-usec = 0

# CSNG params
resample-method = soxr-hq
flat-volumes = no

enable-remixing = yes
enable-lfe-remixing = yes
high-priority = yes
nice-level = -11

default-sample-channels = 2
default-channel-map = front-left,front-right

# Crankshaft detected sample rate
default-sample-rate = 48000

# Crankshaft detected sample format
default-sample-format = s16le
```


- Install Pipewire...
```bash

apt install pipewire-audio pipewire-pulse pipewire-alsa
apt remove pulseaudio-alsa

pactl info # this should show (on PipeWire xxx) under Server Name
```


