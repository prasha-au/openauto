#!/usr/bin/env sh

SCRIPTPATH=$(dirname "$0")

if [ ! -f "$SCRIPTPATH/setup_deps.sh" ]; then
    echo "setup_deps.sh is required in the same folder as this script."
    exit 1
fi

apt-get install -y qemu-user-static debootstrap schroot
wget http://archive.raspbian.org/raspbian.public.key -O - | apt-key add -q


qemu-debootstrap --arch armhf bookworm /mnt/raspihf http://archive.raspbian.org/raspbian/
chroot /mnt/raspihf

cd /root
./setup_deps.sh
