FROM balenalib/raspberrypi3:bookworm-build

ENV DEBIAN_FRONTEND noninteractive

WORKDIR /root

COPY tooling/setup_deps.sh /root/
RUN apt-get update && ./setup_deps.sh && apt-get clean

RUN /usr/bin/env sh
