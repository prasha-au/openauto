FROM ubuntu

RUN apt-get update
RUN apt-get install -y qemu-user-static debootstrap schroot
RUN wget http://archive.raspbian.org/raspbian.public.key -O - | apt-key add -q

RUN qemu-debootstrap --arch armhf bookworm /mnt/raspihf http://archive.raspbian.org/raspbian/
RUN chroot /mnt/raspihf

RUN /bin/bash
RUN export DEBIAN_FRONTEND=noninteractive


RUN apt-get install -y cmake build-essential git


# AASDK depenedncies
RUN apt-get install -y protobuf-compiler libprotobuf-dev libusb-1.0.0-dev libssl-dev libboost-dev libboost-system-dev libboost-log-dev


# we need to add in some qt dependencies at this point for gstreamer
RUN apt-get install -y qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev


# qt-gstreamer
RUN apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev
RUN cd /
RUN git clone --single-branch https://github.com/GStreamer/qt-gstreamer
RUN cd qt-gstreamer

COPY patches/qt-gstreamer-1.18.patch /mnt/raspihf/qt-gstreamer/qt-gstreamer-1.18.patch
COPY patches/qt-gstreamer_greenline_fix.patch /mnt/raspihf/qt-gstreamer/qt-gstreamer_greenline_fix.patch
COPY patches/qt-gstreamer_atomic-load.patch /mnt/raspihf/qt-gstreamer/qt-gstreamer_atomic-load.patch

RUN git apply qt-gstreamer-1.18.patch
RUN git apply qt-gstreamer_greenline_fix.patch
RUN git apply qt-gstreamer_atomic-load.patch

RUN cmake -Bbuild -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH) -DCMAKE_INSTALL_INCLUDEDIR=include -DQT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-std=c++11
RUN cd build
RUN make -j8
RUN make install
RUN ldconfig


# openauto
RUN apt-get install -y libqt5bluetooth5 qtconnectivity5-dev libasound2-dev
RUN cd /
RUN git clone https://github.com/prasha-au/openauto.git
RUN cmake -Bbuild
RUN make -j8

