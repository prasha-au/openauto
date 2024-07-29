import time
import _utils
from threading import Timer
from multiprocessing import Pool, Process, Value, Array, Queue
from enum import Enum

REF_VOLTAGE = 3300
ADC_RANGE_VOLTAGE = 4096
ADC_GAIN_SETTING = 1
ADC_RANGE = 32767


SwcKey = Enum('SwcKey', [
  'Mute', 'VolumeUp', 'VolumeDown', 'Mode', 'SeekForward', 'SeekBackward',
  'PhoneOn', 'PhoneOff', 'Talk'
])


def _from_media_resistance(resistance):
  if 1 <= resistance <= 40:
    return SwcKey.Mute
  elif 60 <= resistance <= 120:
    return SwcKey.VolumeUp
  elif 150 <= resistance <= 250:
    return SwcKey.VolumeDown
  elif 300 <= resistance <= 400:
    return SwcKey.Mode
  elif 500 <= resistance <= 700:
    return SwcKey.SeekForward
  elif 1100 <= resistance <= 1200:
    return SwcKey.SeekBackward
  else:
    return None

def _from_bluetooth_resistance(resistance):
  if 250 <= resistance <= 400:
    return SwcKey.PhoneOn
  elif 900 <= resistance <= 1100:
    return SwcKey.PhoneOff
  elif 3000 <= resistance <= 3400:
    return SwcKey.Talk
  else:
    return None


def _get_retrigger_time(key):
  if key == SwcKey.VolumeUp or key == SwcKey.VolumeDown:
    return 0.25
  else:
    return None



def _calculate_resistance(measurement, base_resistance):
  vout = (measurement / ADC_RANGE) * ADC_RANGE_VOLTAGE
  return (vout * base_resistance) / (REF_VOLTAGE - vout)


def _get_confirmed_measurement(adc, channel):
  prev_measurement = adc.read_adc(channel, gain=ADC_GAIN_SETTING)
  while True:
    time.sleep(0.02)
    curr_measurement = adc.read_adc(channel, gain=ADC_GAIN_SETTING)
    abs_diff = abs(prev_measurement - curr_measurement)
    if abs_diff < 100:
      return curr_measurement
    else:
      prev_measurement = curr_measurement



def _measure_adc(adc_queue):
  from Adafruit_ADS1x15 import ADS1115
  adc = ADS1115()

  last_value = None
  while True:
    measurement = _get_confirmed_measurement(adc, 0)
    resistance = _calculate_resistance(measurement, 330.0)
    suspected_key = _from_media_resistance(resistance)

    if suspected_key is None:
      measurement = _get_confirmed_measurement(adc, 1)
      resistance = _calculate_resistance(measurement, 1500.0)
      suspected_key = _from_bluetooth_resistance(resistance)

    if last_value != suspected_key:
      adc_queue.put(suspected_key)
      last_value = suspected_key


def _handle_swckey(key):
  print(f'we got a key: {key}', flush=True)
  if key == SwcKey.Mute:
    _utils.mute()
  elif key == SwcKey.VolumeUp:
    _utils.increase_volume()
  elif key == SwcKey.VolumeDown:
    _utils.decrease_volume()
  elif key == SwcKey.Mode:
    _utils.send_key(_utils.OpenautoKey.TogglePlay)
  elif key == SwcKey.SeekForward:
    _utils.send_key(_utils.OpenautoKey.NextTrack)
  elif key == SwcKey.SeekBackward:
    _utils.send_key(_utils.OpenautoKey.PreviousTrack)
  elif key == SwcKey.PhoneOn:
    _utils.send_key(_utils.OpenautoKey.Back)
  elif key == SwcKey.PhoneOff:
    _utils.send_key(_utils.OpenautoKey.CallEnd)
  elif key == SwcKey.Talk:
    _utils.send_key(_utils.OpenautoKey.VoiceCommand)



class RepeatTimer(Timer):
  def run(self):
    while not self.finished.wait(self.interval):
      self.function(*self.args, **self.kwargs)

def _handle_keytrigger(adc_queue):
  timer = None
  while True:
    try:
      key = adc_queue.get()
      if timer is not None:
        timer.cancel()

      if key is None:
        continue

      _handle_swckey(key)

      retrigger_time = _get_retrigger_time(key)
      if retrigger_time is not None:
        timer = RepeatTimer(retrigger_time, lambda: _handle_swckey(key))
        timer.start()
    except Exception as e:
      print(f'Failed in keytrigger: {e}')
      if timer is not None:
        timer.cancel()



def main():
  print("Running SWC Key")
  adc_queue = Queue()
  while True:
    try:
      keytrigger_process = Process(target=_handle_keytrigger, args=(adc_queue,))
      keytrigger_process.start()
      _measure_adc(adc_queue)
    except Exception as e:
      print(f'Error: {e}')
      keytrigger_process.terminate()
      time.sleep(1)



if __name__ == '__main__':
  main()
