#!/usr/bin/env sh

set -e


apt-get install -y cmake build-essential git curl \
    protobuf-compiler libprotobuf-dev libusb-1.0.0-dev libssl-dev libboost-dev libboost-system-dev libboost-log-dev \
    qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev qtconnectivity5-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev \
    libasound2-dev


git clone --single-branch https://github.com/GStreamer/qt-gstreamer
cd qt-gstreamer

PATCH_SOURCE="https://github.com/openDsh/dash/raw/develop/patches"
curl -L $PATCH_SOURCE/qt-gstreamer-1.18.patch | git apply -v
curl -L $PATCH_SOURCE/greenline_fix.patch | git apply -v
curl -L $PATCH_SOURCE/qt-gstreamer_atomic-load.patch | git apply -v

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH) \
    -DCMAKE_INSTALL_INCLUDEDIR=include -DQT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-std=c++11

make && make install && ldconfig

cd ..
rm -Rf qt-gstreamer

