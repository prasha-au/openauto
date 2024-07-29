#include <QEvent>
#include <QObject>
#include "./OpenautoEventFilter.hpp"
#include "openauto/Service/AppEvent.hpp"
#include "OpenautoLog.hpp"

namespace autoapp
{
namespace service
{

bool OpenautoEventFilter::eventFilter(QObject *dest, QEvent *event)
{
    if (event->type() != openauto::service::TYPE_OPENAUTO) {
        return QObject::eventFilter(dest, event);
    }
    openauto::service::AppEvent *appEvent = static_cast<openauto::service::AppEvent*>(event);
    OPENAUTO_LOG(info) << "[AppEventFilter] event: " << appEvent->eventType;
    emit onAppEvent(appEvent->eventType);
    return true;
}

}
}
