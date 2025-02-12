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

#include <boost/asio.hpp>
#include "aasdk/Transport/ITransport.hpp"
#include "aasdk/TCP/ITCPEndpoint.hpp"
#include "aasdk/USB/IAOAPDevice.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include "IAndroidAutoEntity.hpp"
#include "ServiceFactory.hpp"

namespace openauto
{
namespace service
{

class AndroidAutoEntityFactory
{
public:
    AndroidAutoEntityFactory(boost::asio::io_service& ioService,
                             configuration::Configuration::Pointer configuration,
                             ServiceFactory& serviceFactory);

    IAndroidAutoEntity::Pointer create(aasdk::usb::IAOAPDevice::Pointer aoapDevice);
    IAndroidAutoEntity::Pointer create(aasdk::tcp::ITCPEndpoint::Pointer tcpEndpoint, bool useBluetooth);

private:
    IAndroidAutoEntity::Pointer create(aasdk::transport::ITransport::Pointer transport, bool useBluetooth);

    boost::asio::io_service& ioService_;
    configuration::Configuration::Pointer configuration_;
    ServiceFactory& serviceFactory_;
};

}
}
