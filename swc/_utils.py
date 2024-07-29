import subprocess
from enum import Enum
import re
import math

AUDIO_PIN = 17

# See openauto/Projection/InputDevice.cpp
class OpenautoKey:
  Play = 'X'
  Pause = 'C'
  TogglePlay = 'B'
  NextTrack = 'N'
  PreviousTrack = 'V'
  Enter = 'Return'
  Home = 'H'
  Phone = 'P'
  CallEnd = 'O'
  VoiceCommand = 'M'
  Left = 'Left'
  Right = 'Right'
  Up = 'Up'
  Down = 'Down'
  Back = 'Escape'
  ScrollUp = '1'
  ScrollDown = '2'


def _amixer_set(device, adjustment):
  command = ['amixer', '-q', 'set', device, adjustment]
  return subprocess.run(command).returncode == 0

def _amixer_set_master(adjustment):
  return _amixer_set('Master', adjustment)

def increase_volume():
  _amixer_set_master('5%+')

def decrease_volume():
  _amixer_set_master('5%-')

def mute():
  _amixer_set_master('mute')

def unmute():
  _amixer_set_master('unmute')

def toggle_mute():
  _amixer_set_master('toggle')

def get_volume():
  command = ['amixer', 'sget', 'Master']
  result = subprocess.run(command, capture_output = True, text = True)
  matches = re.search("Playback\s\d+\s\[(\d+)%\]\s\[(on|off)\]", result.stdout)
  return { 'muted': matches.group(2) == 'off', 'level': math.floor(int(matches.group(1))) }

def send_key(key):
  with open('inject_keypress.fifo', 'a') as fifo:
    fifo.write(f"{key}\n")
