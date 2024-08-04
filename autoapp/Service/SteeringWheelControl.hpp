
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
    float readAdcVoltage(int);
    Qt::Key determineKey();

signals:
    void onKeyPress(Qt::Key);



};


}
}
