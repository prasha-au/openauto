/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QObject>
#include <QKeyEvent>
#include "IInputDevice.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include <bits/stdc++.h>

namespace openauto
{
namespace projection
{

class InputDevice: public QObject, public IInputDevice, boost::noncopyable
{
    Q_OBJECT

public:
    InputDevice(QObject& parent, configuration::Configuration::Pointer configuration, const QRect& touchscreenGeometry, const QRect& videoGeometry);

    void start(IInputDeviceEventHandler& eventHandler) override;
    void stop() override;
    ButtonCodes getSupportedButtonCodes() const override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    bool hasTouchscreen() const override;
    QRect getTouchscreenGeometry() const override;
    void setTouchscreenGeometry(QRect& touchscreenGeometry);

private:
    void setVideoGeometry();
    bool handleKeyEvent(QEvent* event, QKeyEvent* key);
    void dispatchKeyEvent(ButtonEvent event);
    bool handleTouchEvent(QEvent* event);
    bool handleMouseEvent(QEvent* event);

    QObject& parent_;
    configuration::Configuration::Pointer configuration_;
    QRect touchscreenGeometry_;
    QRect displayGeometry_;
    IInputDeviceEventHandler* eventHandler_;
    std::mutex mutex_;

    std::priority_queue <int, std::vector<int>, std::greater<int> > pointer_id_queue;
    QMap<int, int> pointer_map;
    int max_pointers = 0;
};

}
}
