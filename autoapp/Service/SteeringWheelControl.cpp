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
        {Qt::Key_VolumeUp, { 0.600, 1.000 }},
        {Qt::Key_VolumeDown, { 1.200, 1.500 }},
        {Qt::Key_H, { 1.600, 1.650 }},  // hook on => home
        {Qt::Key_B, { 1.660, 1.900 }},  // mode => play/pause
        {Qt::Key_N, { 2.000, 2.300 }},  // seek+ => media next
        {Qt::Key_Escape, { 2.330, 2.450 }},  // hook off
        {Qt::Key_V, { 2.500, 2.700 }},  // seek- => media prev
        {Qt::Key_M, { 2.800, 2.900 }},  // talk
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
        float voltage = readAdcVoltage();
        Qt::Key newKey = fromVoltageReading(voltage);
        if (lastKey == Qt::Key_unknown && newKey != Qt::Key_unknown) {
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
    ) {
        close(i2cFileDescriptor_);
        return false;
    }
    return true;
}


float SteeringWheelControl::readAdcVoltage()
{
    const int JITTER_THRESHOLD = 100; // 0.0125V

    int stableValue = 0;
    int verifyCount = 0;
    while (verifyCount < 5) {
        uint8_t valueBuf[2];
        if (read(i2cFileDescriptor_, valueBuf, 2) != 2) {
            OPENAUTO_LOG(warning) << "[SteeringWheelControl] Failed to read ADC value";
            stableValue = 0;
            verifyCount = 0;
            QThread::msleep(100);
            continue;
        }
        int value = (valueBuf[0] << 8) + valueBuf[1];
        if (std::abs(value - stableValue) < JITTER_THRESHOLD) {
            verifyCount++;
        } else {
            stableValue = value;
            verifyCount = 0;
        }
        QThread::msleep(8);
    }

    float voltage = (stableValue / 32768.0) * 4.096;
    return voltage;
}

}
}
