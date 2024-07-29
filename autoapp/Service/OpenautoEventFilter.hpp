#pragma once

#include <QEvent>
#include <QObject>
#include "openauto/Service/AppEvent.hpp"

namespace autoapp
{
namespace service
{

class OpenautoEventFilter : public QObject
{
    Q_OBJECT

protected:
    bool eventFilter( QObject *dest, QEvent *event );
signals:
    void onAppEvent(openauto::service::AppEventType);
};

}
}
