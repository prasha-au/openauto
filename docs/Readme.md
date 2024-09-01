## config.txt
```txt
dtoverlay=vc4-fkms-v3d
gpu_mem=256

boot_delay=0
initial_turbo=30
start_cd=1
dtoverlay=sdtweak,overclock_50=100

disable_splash=1
dtparam=i2c_arm=on
dtoverlay=pi3-disable-wifi
```

## cmdline.txt
Note the root partition in case the SD card has been made differently. The one below matches RPi imager with Raspbian Bookworm.
```
console=tty3 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=noop fsck.mode=skip fsck.repair=yes rootwait quiet logo.nologo loglevel=0 noswap plymouth.ignore-serial-consoles consoleblank=0 ipv6.disable=1 cfg80211.ieee802 cfg80211.ieee80211_regdom=AU
```


## setup 5ghz hotspot
Set wifi reg country first via raspi-config.

```bash
nmcli con delete preconfigured

nmcli con delete hotspot
nmcli con add type wifi ifname wlan0 mode ap con-name hotspot ssid dash autoconnect true
nmcli con modify hotspot 802-11-wireless.band a
nmcli con modify hotspot 802-11-wireless.channel 165
nmcli con modify hotspot ipv4.method shared ipv4.address 192.168.4.1/24
nmcli con modify hotspot ipv6.method disabled
nmcli con modify hotspot wifi-sec.key-mgmt wpa-psk
nmcli con modify hotspot wifi-sec.psk "xxxxxxxxxx"
nmcli con up hotspot
```


## login speedup
```
dphys-swapfile swapoff
dphys-swapfile uninstall
nano /etc/dphys-swapfile
systemctl disable dphys-swapfile.service

systemctl disable systemd-timesyncd.service
systemctl disable ModemManager.service

systemctl disable udisks2.service
systemctl disable rp1-test.service
systemctl mask systemd-rfkill.service     # hw button for wifi
systemctl disable triggerhappy.service    # joystick stuff or something on /dev/input/
systemctl disable raspi-config.service    # reenable if using some options in raspi-config
systemctl disable glamor-test.service     # unsure but related to x11
systemctl mask rc-local                   # if not using /etc/rc.local
systemctl disable sshswitch.service       # raspi enable ssh if file in boot
systemctl disable rpi-eeprom-update.service  # applies a bootloader update if available
systemctl disable systemd-journal-flush.service   # disable journal flushing from previous boot
systemctl disable keyboard-setup.service  # keyboard locale stuff

```


## login messages
Change `/etc/systemd/system/getty@tty1.service.d/autologin.conf`
```
[Service]
ExecStart=
ExecStart=-/sbin/agetty --skip-login --nonewline --noissue --autologin [User Name] --noclear %I $TERM
```

Add `~/.hushlogin`.


## pipewire
Pipewire got rid of audio lag I was getting with PulseAudio. The interface libs add a direct dropin replacement for PulseAudio.
```bash
apt install pipewire-audio pipewire-pulse pipewire-alsa
apt remove pulseaudio-alsa

pactl info # this should show (on PipeWire xxx) under Server Name
```
