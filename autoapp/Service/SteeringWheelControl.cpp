
#include <iostream>
#include "OpenautoLog.hpp"
#include "SteeringWheelControl.hpp"
#include "OpenautoLog.hpp"
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>


namespace autoapp
{
namespace service
{

namespace {

int calculateResistance(float vout, int base_resistance)
{
  return (vout * base_resistance) / (3.3000 - vout);
}

Qt::Key fromMediaMeasurement(float adcVoltage)
{
    int resistance = calculateResistance(adcVoltage, 330);
    const std::map<Qt::Key, std::pair<int, int>> KEY_MAP = {
        {Qt::Key_VolumeMute, { 1, 40 }},
        {Qt::Key_VolumeUp, { 60, 120 }},
        {Qt::Key_VolumeDown, { 150, 250 }},
        {Qt::Key_MediaPlay, { 300, 400 }},  // Mode
        {Qt::Key_MediaNext, { 500, 700 }},
        {Qt::Key_MediaPrevious, { 1100, 1200 }},
    };
    for (const auto& pair : KEY_MAP) {
        if (pair.second.first <= resistance && resistance <= pair.second.second) {
            return pair.first;
        }
    }
    return Qt::Key_unknown;
}

Qt::Key fromHandsFreeMeasurement(float adcVoltage)
{
    int resistance = calculateResistance(adcVoltage, 1500);
    const std::map<Qt::Key, std::pair<int, int>> KEY_MAP = {
        {Qt::Key_H, { 250, 400 }},             // Phone on
        {Qt::Key_Escape, { 900, 1100 }},      // Phone off
        {Qt::Key_M, { 3000, 3400 }},          // Talk
    };
    for (const auto& pair : KEY_MAP) {
        if (pair.second.first <= resistance && resistance <= pair.second.second) {
            return pair.first;
        }
    }
    return Qt::Key_unknown;
}

}


void SteeringWheelControl::run()
{
    int8_t testBuf = { 0 };
    if (
        (i2cFileDescriptor_ = open("/dev/i2c-1", O_RDWR)) < 0 ||
        ioctl(i2cFileDescriptor_, I2C_SLAVE, 0x48) < 0 ||
        read(i2cFileDescriptor_, &testBuf, 1) < 1
    ) {
        OPENAUTO_LOG(error) << "[SteeringWheelControl] Failed to initialize ADC";
        close(i2cFileDescriptor_);
        return;
    }


    Qt::Key lastKey = Qt::Key_unknown;
    while (true) {
        Qt::Key newKey = determineKey();
        if (lastKey == Qt::Key_unknown && newKey != Qt::Key_unknown) {
            OPENAUTO_LOG(info) << "[SteeringWheelControl] Key pressed: " << newKey;
            emit onKeyPress(newKey);
        }
        lastKey = newKey;
    }
}


Qt::Key SteeringWheelControl::determineKey()
{
    Qt::Key mediaKey = fromMediaMeasurement(readAdcVoltage(0));
    if (mediaKey != Qt::Key_unknown) {
        return mediaKey;
    } else {
        Qt::Key handsFreeKey = fromHandsFreeMeasurement(readAdcVoltage(1));
        return handsFreeKey;
    }
}



float SteeringWheelControl::readAdcVoltage(int channel)
{
  const uint8_t adcChannelBits[] = { 0b100, 0b101, 0b110, 0b111 };

  // ADC config package
  // bit    | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  |
  // byte 1 | op |      adc #   |     gain     | op |
  // byte 2 |  data rate   |    comparator opts     |
  uint8_t configBuf[] = {
    1,
    (uint8_t) (0b10000011 | (adcChannelBits[channel] << 4)),
    0b10100000,
  };


  int stableValue = -1;
  for (int i = 0; i < 5; i++) {
    try {
        if (write(i2cFileDescriptor_, configBuf, 3) != 3) { throw -1; }

        uint8_t isReadyCheck[1] = { 0 };
        do {
            if (read(i2cFileDescriptor_, &isReadyCheck, 1) != 1) { throw -2; }
        } while ((isReadyCheck[0] & 0b10000000) == 0);


        const uint8_t regSelect[1] = { 0 };
        if (write(i2cFileDescriptor_, &regSelect, 1) != 1) { throw -3; }

        uint8_t valueBuf[2];

        if (read(i2cFileDescriptor_, valueBuf, 2) != 2) { throw -4; }

        int value = (valueBuf[0] << 8) + valueBuf[1];
        stableValue = stableValue == -1 ? value : ((stableValue + value) / 2);


    } catch (int errorNumber) {
        OPENAUTO_LOG(warning) << "Failed to read ADC " << errorNumber;
    }
  }

  float vout = (stableValue / 32768.0) * 4.096;
  return vout;
}

}
}
