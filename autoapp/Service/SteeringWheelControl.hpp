
#pragma once

#include <QObject>
#include <QThread>

namespace autoapp
{
namespace service
{


class SteeringWheelControl : public QThread
{
    Q_OBJECT

private:
    void run();
    int i2cFileDescriptor_;
    bool setupAdc();
    float readAdcVoltage();

signals:
    void onKeyPress(Qt::Key);
    void onAdcUpdate(float);

};


}
}
