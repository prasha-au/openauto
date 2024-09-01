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

Qt::Key fromVoltageReading(float adcVoltage) {
    const std::map<Qt::Key, std::pair<float, float>> KEY_MAP = {
        {Qt::Key_VolumeUp, { 0.700, 0.750 }},
        {Qt::Key_VolumeDown, { 1.210, 1.260 }},
        {Qt::Key_H, { 1.590, 1.640 }},  // hook on
        {Qt::Key_MediaPlay, { 1.660, 1.710 }},  // mode
        {Qt::Key_MediaNext, { 2.120, 2.175 }},
        {Qt::Key_Escape, { 2.350, 2.400 }},  // hook off
        {Qt::Key_MediaPrevious, { 2.540, 2.600 }},
        {Qt::Key_M, { 2.790, 2.840 }},  // talk
    };
    for (const auto& pair : KEY_MAP) {
        if (pair.second.first <= adcVoltage && adcVoltage <= pair.second.second) {
            return pair.first;
        }
    }
    return Qt::Key_unknown;
}

}


void SteeringWheelControl::run()
{
    if (!setupAdc()) {
        OPENAUTO_LOG(error) << "[SteeringWheelControl] Failed to initialize ADC";
        return;
    }

    Qt::Key lastKey = Qt::Key_unknown;
    while (true) {
        float adcReading = readAdcVoltage();
        // OPENAUTO_LOG(info) << "[SteeringWheelControl] ADC Voltage: " << adcReading;
        Qt::Key newKey = fromVoltageReading(adcReading);
        if (lastKey == Qt::Key_unknown && newKey != Qt::Key_unknown) {
            OPENAUTO_LOG(info) << "[SteeringWheelControl] Key pressed: " << newKey;
            emit onKeyPress(newKey);
        }
        lastKey = newKey;
    }
}



bool SteeringWheelControl::setupAdc()
{
    // ADC config package
    // bit    | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  |
    // byte 1 | op |      adc #   |     gain     | op |
    // byte 2 |  data rate   |    comparator opts     |

    // Current config is: continuous read mode, channel 1, 250 samples/sec, 4.096V range
    const uint8_t configBuf[] = { 1, 0b11010010, 0b10000000 };
    const uint8_t regSelectBuf[1] = { 0 };
    if (
        (i2cFileDescriptor_ = open("/dev/i2c-1", O_RDWR)) < 0 ||
        ioctl(i2cFileDescriptor_, I2C_SLAVE, 0x48) < 0 ||
        write(i2cFileDescriptor_, configBuf, 3) != 3 ||
        write(i2cFileDescriptor_, &regSelectBuf, 1) != 1
        // TODO: Disconnect ADC on initialized i2c bus to check if this correctly fails. Failing that we may need to do a read to verify presence.
    ) {
        close(i2cFileDescriptor_);
        return false;
    }
    return true;
}


float SteeringWheelControl::readAdcVoltage()
{
  int averageValue = 0;
  for (int i = 0; i < 5; i++) {
    uint8_t valueBuf[2];
    if (read(i2cFileDescriptor_, valueBuf, 2) != 2) {
        OPENAUTO_LOG(warning) << "[SteeringWheelControl] Failed to read ADC value";
        return -1;
    }
    int value = (valueBuf[0] << 8) + valueBuf[1];
    averageValue = i == 0 ? value : ((averageValue + value) / 2);
    QThread::msleep(10);
  }


  float voltage = (averageValue / 32768.0) * 4.096;
  emit onAdcUpdate(voltage);
  return voltage;
}

}
}
