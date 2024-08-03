FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get install -y cmake build-essential git \
        # deps for AASDK build
        protobuf-compiler libprotobuf-dev libusb-1.0.0-dev libssl-dev libboost-dev libboost-system-dev libboost-log-dev \
        # QT deps for build
        qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev qtconnectivity5-dev \
        # deps for gstreamer build
        libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev \
        # deps for openauto
        libasound2-dev \
        # runtime deps
        libqt5multimedia5 libqt5multimedia5-plugins \
        libqt5multimediawidgets5 libqt5bluetooth5 libqt5bluetooth5-bin qml-module-qtquick2 qml-module-qtmultimedia \
        gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-alsa \
        pulseaudio alsa-utils && \
    apt-get clean


WORKDIR /root

## Build & install qt-gstreamer
COPY patches/qt-gstreamer-1.18.patch patches/
COPY patches/qt-gstreamer_greenline_fix.patch patches/
COPY patches/qt-gstreamer_atomic-load.patch patches/

RUN git clone --single-branch https://github.com/GStreamer/qt-gstreamer && \
    cd qt-gstreamer && \
    git apply ../patches/qt-gstreamer-1.18.patch && \
    git apply ../patches/qt-gstreamer_greenline_fix.patch  && \
    git apply ../patches/qt-gstreamer_atomic-load.patch && \
    cmake -Bbuild -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH) \
        -DCMAKE_INSTALL_INCLUDEDIR=include -DQT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-std=c++11  && \
    cd build && make && make install && ldconfig && \
    cd /root && rm -Rf /root/qt-gstreamer


WORKDIR /workspaces


