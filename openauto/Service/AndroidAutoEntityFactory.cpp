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

#include "aasdk/USB/AOAPDevice.hpp"
#include "aasdk/Transport/SSLWrapper.hpp"
#include "aasdk/Transport/USBTransport.hpp"
#include "aasdk/Transport/TCPTransport.hpp"
#include "aasdk/Messenger/Cryptor.hpp"
#include "aasdk/Messenger/MessageInStream.hpp"
#include "aasdk/Messenger/MessageOutStream.hpp"
#include "aasdk/Messenger/Messenger.hpp"
#include "openauto/Service/AndroidAutoEntityFactory.hpp"
#include "openauto/Service/AndroidAutoEntity.hpp"
#include "openauto/Service/Pinger.hpp"
#include "OpenautoLog.hpp"

namespace openauto
{
namespace service
{

AndroidAutoEntityFactory::AndroidAutoEntityFactory(boost::asio::io_service& ioService,
                                                   configuration::Configuration::Pointer configuration,
                                                   ServiceFactory& serviceFactory)
    : ioService_(ioService)
    , configuration_(std::move(configuration))
    , serviceFactory_(serviceFactory)
{

}

IAndroidAutoEntity::Pointer AndroidAutoEntityFactory::create(aasdk::usb::IAOAPDevice::Pointer aoapDevice)
{
    auto transport(std::make_shared<aasdk::transport::USBTransport>(ioService_, std::move(aoapDevice)));
    return create(std::move(transport), false);
}

IAndroidAutoEntity::Pointer AndroidAutoEntityFactory::create(aasdk::tcp::ITCPEndpoint::Pointer tcpEndpoint, bool useBluetooth)
{
    auto transport(std::make_shared<aasdk::transport::TCPTransport>(ioService_, std::move(tcpEndpoint)));
    return create(std::move(transport), useBluetooth);
}

IAndroidAutoEntity::Pointer AndroidAutoEntityFactory::create(aasdk::transport::ITransport::Pointer transport, bool useBluetooth)
{
    auto sslWrapper(std::make_shared<aasdk::transport::SSLWrapper>());
    auto cryptor(std::make_shared<aasdk::messenger::Cryptor>(std::move(sslWrapper)));
    cryptor->init();

    auto messenger(std::make_shared<aasdk::messenger::Messenger>(ioService_,
                                                                 std::make_shared<aasdk::messenger::MessageInStream>(ioService_, transport, cryptor),
                                                                 std::make_shared<aasdk::messenger::MessageOutStream>(ioService_, transport, cryptor)));

    auto serviceList = serviceFactory_.create(messenger, useBluetooth);
    auto pinger(std::make_shared<Pinger>(ioService_, 5000));
    return std::make_shared<AndroidAutoEntity>(ioService_, std::move(cryptor), std::move(transport), std::move(messenger), configuration_, std::move(serviceList), std::move(pinger));
}

}
}
