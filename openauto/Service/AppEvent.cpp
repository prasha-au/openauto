
#include "openauto/Service/AppEvent.hpp"
#include <QEvent>
#include <QApplication>

namespace openauto
{
namespace service
{

const QEvent::Type TYPE_OPENAUTO = static_cast<QEvent::Type>(QEvent::registerEventType());

AppEvent::AppEvent(AppEventType eventType) : QEvent(TYPE_OPENAUTO), eventType(eventType) {}

void emitAppEvent(AppEventType eventType)
{
    QCoreApplication *app = QApplication::instance();
    app->postEvent(app, new AppEvent(eventType));
}


}
}
