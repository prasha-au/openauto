FROM ubuntu AS bootstrap

ENV DEBIAN_FRONTEND noninteractive

WORKDIR /root

RUN apt-get update && apt-get install -y qemu-user-static debootstrap schroot && apt-get clean
RUN wget http://archive.raspbian.org/raspbian.public.key -O - | apt-key add -q
RUN qemu-debootstrap --arch armhf bookworm /mnt/raspi http://archive.raspbian.org/raspbian/

RUN chroot /mnt/raspi


FROM scratch
COPY --from=bootstrap /mnt/raspi /

ENV DEBIAN_FRONTEND noninteractive

WORKDIR /root

COPY tooling/setup_deps.sh /root

RUN apt-get update && ./setup_deps.sh && apt-get clean
