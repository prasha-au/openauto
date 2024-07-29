#pragma once

#include <QObject>
#include <QThread>
#include <alsa/asoundlib.h>

namespace autoapp
{
namespace service
{

class KeyReceiver : public QThread
{
    Q_OBJECT

private:
    void run();

signals:
    void onKeyPress(int);

};


}
}
