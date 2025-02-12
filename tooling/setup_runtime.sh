#!/usr/bin/env sh

set -e


apt-get install -y libqt5multimedia5 libqt5multimedia5-plugins \
    libqt5multimediawidgets5 libqt5bluetooth5 libqt5bluetooth5-bin qml-module-qtquick2 qml-module-qtmultimedia \
    gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-alsa \
    alsa-utils pulseaudio pulseaudio-module-bluetooth ofono


# udev rule for usb access
echo "SUBSYSTEM==\"usb\", ATTR{idVendor}==\"*\", ATTR{idProduct}==\"*\", MODE=\"0660\", GROUP=\"plugdev\"" | tee /etc/udev/rules.d/51-openauto.rules

# allow ofono to push onto dbus properly
sed -i 's/deny send_destination="org.ofono"/allow send_destination="org.ofono"/g' /etc/dbus-1/system.d/ofono.conf

# allow pulseaudio to grab realtime priority
tee -a /etc/security/limits.d/openauto.conf <<EOT
@audio - nice -20
@audio - rtprio 95
@audio - memlock unlimited
EOT

# enable pulseaudio bluetooth module with the fixed ofono headset option
sed -i 's/load-module module-bluetooth-discover/load-module module-bluetooth-discover headset=ofono/g' /etc/pulse/default.pa


# echo/noise cancellation for pulseaudio
tee -a /etc/pulse/default.pa <<EOT

### Echo cancel and noise reduction
.ifexists module-echo-cancel.so
load-module module-echo-cancel aec_method=webrtc source_name=ec_out sink_name=ec_ref
set-default-source ec_out
set-default-sink ec_ref
.endif
EOT
