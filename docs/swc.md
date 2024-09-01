# Steering Wheel Control
This facilitates the use of Subaru Steering Wheel Controls (SWC) with the Pi.


## SWC Resistance Values
For a MY10 Subaru Liberty the resistance values can be found in [this thread](https://www.legacygt.com/topic/66617-2010-aftermarket-nav-steering-wheel-controls-pac-swi-ps-and-pioneer/).
It comes in 2 parts for Bluetooth based units so we will wire them up in series with a voltage divider. This allows us to run continuous read on the ADC without switching channels.

This is the calculated (C) and actual (A) resistances and voltage readings from the ADC given 3.3v in and a 330Ω resistor.

| Button    | SWC1 | SWC2  | C Res | C Vout | A Res | A Vout |
|-----------|------|-------|-------|--------|-------|--------|
| none      | 4700 | 100000| 4489  | 3.074  | 4667  |  3.090 |
| mute      | 22   | 100000| 22    | 0.206  |       |        |
| vol+      | 90   | 100000| 90    | 0.707  | 260   |  0.727 |
| vol-      | 200  | 100000| 200   | 1.244  | 365   |  1.238 |
| mode      | 360  | 100000| 359   | 1.719  | 510   |  1.681 |
| seek+     | 690  | 100000| 685   | 2.227  | 770   |  2.148 |
| seek-     | 1159 | 100000| 1146  | 2.562  | 1300  |  2.572 |
| hook on   | 4700 | 330   | 308   | 1.594  | 485   |  1.616 |
| hook off  | 4700 | 1000  | 825   | 2.357  | 997   |  2.375 |
| talk      | 4700 | 3200  | 1904  | 2.812  | 2047  |  2.823 |



## Development

### Verify I2C
You can test that I2C is enabled and working on the Pi using the instructions below.
```bash
// check i2c is enabled
raspi-config nonint get_i2c

// check for i2c node
ls /dev/i2c*

// check that i2c modules are loaded
lsmod | grep i2c

// scan for the i2c device
i2cdetect -y 1

// read something from the device on the default address
i2cget -y 1 0x48
```

### Hardware
- ADS1115
- 330Ω resistor
- 1.5kΩ resistor


Refer to [Fritzing file](./assets/swc.fzz) or [this Falstad link](https://falstad.com/circuit/circuitjs.html?ctz=CQAgjCAMB0l3BWcMBMcUHYMGZIA4UA2ATmIxAUgpABZsKBTAWjDACgBnEPKp7NEGjwg+AmuBAAzAIYAbDgxABZAKoAVAKKduvTCkH4Re2hJnzFANQDyAGQA6AB208jKcUNfjxEMwpDWbJmdeMAxCA2EWMJMfOT8lKwARLS4XFhRhD3Thbyk4xQBlDQ0AaUc2AHcdEX4qNONISuqmY3q3KCa28TTQ8MaqnuiejI6AJwiawmFayeEqMEbxrOMsmaoMRYmW9uWMfXnQtiXDKPDl9vm8dmPIsBGs3qhaMGIjkBm+KfeBUSeaDf6314-BAhBOIMBH2wXxoNEi0Lmb1WX1WAnmKE6vBcaQhmJq+ih+zYAA8QMR9Hw9uBCBBPsQTO0CgB1ADC7FJ5JA0TANK59Pc4mZLIxAyoGWmAnFYwmLg8dzmgmw7CqMxcsOELkhkrwOThgh10tVVBmUvm8DgSMMKB2hkeYtwmzl0TlIyoU0dhhcRqeC0BLkesouePl1TtwUENsy7VyvkUagAgjYSuHHk7wjH8iAAAoACSsADkNHYOFYAGKllP3W0jDPmbN5wvFgt463dMUGzYYBAUsCw7h6lh9424Jrq-XCMGZDtsBFc7siXtt3QXDqpLFik5q0yZoqlIKihcjNKay23KuRTVckcAN378P0eAHi6eVBoxug9CoX+gCFHerVA6zoCY6fMIXY9kOTSTuOoLgkSVTQb8j73h0AD27wSk8xJXJAdzvNA1o8HgxA8kQxBwoQhDkDA8AQGKUA-iIxBINg0wMeaKBII0QA) for wiring details.



