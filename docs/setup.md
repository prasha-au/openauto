
# Image setup
Guide to a manual image setup.


## 1. The image
Use Pi Imager to setup a base image. I used the username `oa` for this.


## 2. Base requirements
1. Setup increased swap space to avoid compile errors:
```bash
dphys-swapfile swapoff
sed -i 's/CONF_SWAPSIZE=100/CONF_SWAPSIZE=1024/g' /etc/dphys-swapfile
dphys-swapfile setup
dphys-swapfile swapon
```
2. `apt-get update`
3. Run the `setup_deps.sh` and `setup_runtime.sh` scripts.


## 3. Install OpenAuto
1. Cross compile and copy the files to `~/openauto`.


## 4. Enable auto login and start
1. Enable autostart via raspi-config
2. Add the following lines to the top of `~/.bashrc`
```bash
if [[ "$(tty)" == "/dev/tty1" ]]; then
  pinctrl set 26 op
  pinctrl set 26 dh
  cd /home/oa/openauto
  while true; do
    ./autoapp --platform eglfs
    sleep 1
  done
fi
```


## 5. Bootup speed + improvements
1. Edit the `config.txt` file with the following changes:
```txt
dtoverlay=vc4-fkms-v3d
gpu_mem=256

boot_delay=0
initial_turbo=30
start_cd=1
dtoverlay=sdtweak,overclock_50=100

disable_splash=1
dtparam=i2c_arm=on
#dtoverlay=pi3-disable-wifi
dtoverlay=pi3-disable-bt

avoid_warnings=1

gpio=26=op,dl
```
2. Edit the `cmdline.txt` file with the following changes:
```text
console=tty1 root=xxxxxxx rootfstype=ext4 elevator=noop fsck.mode=skip fsck.repair=yes rootwait quiet logo.nologo loglevel=0 noswap plymouth.ignore-serial-consoles consoleblank=0 ipv6.disable=1 cfg80211.ieee802 cfg80211.ieee80211_regdom=AU
```
3. Turn of unncessary services. You may have to edit this list by using `systemd-analyze blame`
```bash
systemctl disable dphys-swapfile.service
systemctl disable systemd-timesyncd.service
systemctl disable ModemManager.service
systemctl disable NetworkManager-wait-online.service
systemctl disable udisks2.service
systemctl mask systemd-rfkill.service           # hw button for wifi
systemctl mask systemd-rfkill.socket
systemctl disable triggerhappy.service          # joystick stuff or something on /dev/input/
systemctl disable triggerhappy.socket
systemctl disable raspi-config.service          # reenable if using some options in raspi-config
systemctl mask rc-local                         # if not using /etc/rc.local
systemctl disable sshswitch.service             # raspi enable ssh if file in boot
systemctl disable rpi-eeprom-update.service     # applies a bootloader update if available
systemctl disable keyboard-setup.service        # keyboard locale stuff
systemctl disable e2scrub_reap.service
systemctl disable console-setup.service
systemctl disable avahi-daemon.service
systemctl disable fake-hwclock.service
systemctl mask systemd-journal-flush.service
sysetmctl mask systemd-fsck-root.service
```
4. Change SSH to not use DNS in `/etc/ssh/sshd_config`
5. Hush login by running `touch ~/.hushlogin`.
6. Turn off login messages and enable auto login by changing `/etc/systemd/system/getty@tty1.service.d/autologin.conf`.
```text
ExecStart=-/sbin/agetty --skip-login --nonewline --noissue --autologin oa --noclear %I $TERM
```

## 6. Wireless Setup
1. Set wifi regulatory domain via `raspi-config`
2. Setup hotspot via nmcli
```bash
nmcli con delete preconfigured
nmcli con delete hotspot
nmcli con add type wifi ifname wlan0 mode ap con-name hotspot ssid openauto autoconnect true
nmcli con modify hotspot 802-11-wireless.band bg
nmcli con modify hotspot 802-11-wireless.channel 10
nmcli con modify hotspot ipv4.method shared ipv4.address 192.168.4.1/24
nmcli con modify hotspot ipv6.method disabled
nmcli con modify hotspot wifi-sec.key-mgmt wpa-psk
nmcli con modify hotspot wifi-sec.pairwise ccmp
nmcli con modify hotspot wifi-sec.group ccmp
nmcli con modify hotspot wifi-sec.psk "xxxxxxxx"
nmcli con modify hotspot wifi-sec.pmf 0
nmcli con up hotspot
```
3. Add the corresponding lines to `openauto.ini`
```ini
[WiFi]
SSID=openauto
Password=xxxxxx
```
4. Pair and trust the phone via `bluetoothctl`


# References
- https://hackaday.io/project/165208-an-old-rotary-phone-as-bluetooth-set/log/162491-setting-up-the-bluetooth

