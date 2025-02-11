#pragma once

#include <QApplication>
#include <QEvent>
#include "OpenautoLog.hpp"

namespace openauto
{
namespace service
{

enum AppEventType
{
    ProjectionShow,
    ProjectionEnd,
    AndroidAutoStopped,
    BluetoothConnected,
    WirelessConnecting,
};

extern const QEvent::Type TYPE_OPENAUTO;

class AppEvent : public QEvent
{
public:
    AppEventType eventType;
    explicit AppEvent(AppEventType);
};

extern void emitAppEvent(AppEventType);

}
}
