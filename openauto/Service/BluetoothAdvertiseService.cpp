#include <QNetworkInterface>
#include <QThread>
#include "OpenautoLog.hpp"
#include "openauto/Service/BluetoothAdvertiseService.hpp"
#include <QBluetoothLocalDevice>
#include <QProcess>

namespace openauto
{
namespace service
{

BluetoothAdvertiseService::BluetoothAdvertiseService(configuration::Configuration::Pointer config)
    : server_(std::make_unique<QBluetoothServer>(QBluetoothServiceInfo::RfcommProtocol, this))
    , socket_(nullptr)
    , config_(std::move(config))
{
    connect(server_.get(), &QBluetoothServer::newConnection, this, &BluetoothAdvertiseService::onClientConnected);
}




void BluetoothAdvertiseService::startAdvertising()
{

    auto adapters = QBluetoothLocalDevice::allDevices();
    if(adapters.size() < 1)
    {
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] no adapter found";
        return;
    }

    QBluetoothAddress adapterAddress = adapters.at(0).address();


    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] advertising";


    //"4de17a00-52cb-11e6-bdf4-0800200c9a66";
    //"669a0c20-0008-f4bd-e611-cb52007ae14d";
    const QBluetoothUuid serviceUuid(QLatin1String("4de17a00-52cb-11e6-bdf4-0800200c9a66"));

    serviceInfo_ = QBluetoothServiceInfo();

    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
    serviceInfo_.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList, classId);
    classId.prepend(QVariant::fromValue(serviceUuid));
    serviceInfo_.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
    serviceInfo_.setAttribute(QBluetoothServiceInfo::ServiceName, "OpenAuto Bluetooth Service");
    serviceInfo_.setAttribute(QBluetoothServiceInfo::ServiceDescription, "AndroidAuto WiFi projection automatic setup");
    serviceInfo_.setAttribute(QBluetoothServiceInfo::ServiceProvider, "openDsh");
    serviceInfo_.setServiceUuid(serviceUuid);

    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    serviceInfo_.setAttribute(QBluetoothServiceInfo::BrowseGroupList, publicBrowse);

    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(22));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo_.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, protocolDescriptorList);


    serviceInfo_.registerService(adapterAddress);
    server_->listen(adapterAddress, 22);

}

void BluetoothAdvertiseService::onClientConnected()
{
    socket_ = server_->nextPendingConnection();
    if(socket_ != nullptr)
    {
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Device Connected: " << socket_->peerName().toStdString();
        connect(socket_, SIGNAL(readyRead()), this, SLOT(readSocket()));
        writeSocketInfoRequest();
    }
    else
    {
        OPENAUTO_LOG(error) << "[BluetoothAdvertiseService] received null socket during client connection.";
    }
}

bool BluetoothAdvertiseService::writeProtoMessage(uint16_t messageType, google::protobuf::Message& message)
{
    QByteArray byteArray(message.SerializeAsString().c_str(), message.ByteSize());
    uint16_t messageLength = message.ByteSize();
    byteArray.prepend(messageType & 0x000000ff);
    byteArray.prepend((messageType & 0x0000ff00) >> 8);
    byteArray.prepend(messageLength & 0x000000ff);
    byteArray.prepend((messageLength & 0x0000ff00) >> 8);

    if(socket_->write(byteArray) != byteArray.length())
    {
        return false;
    }

    return true;
}

void BluetoothAdvertiseService::writeSocketInfoRequest()
{
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Sending SocketInfoRequest.";

    QString ipAddr;
    foreach(QHostAddress addr, QNetworkInterface::allAddresses())
    {
        if(!addr.isLoopback() && (addr.protocol() == QAbstractSocket::IPv4Protocol))
        {
            ipAddr = addr.toString();
        }
    }

    extraprotos::SocketInfoRequest socketInfoRequest;
    socketInfoRequest.set_ip_address(ipAddr.toStdString());
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] ipAddress: "<< ipAddr.toStdString();

    socketInfoRequest.set_port(5000);
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] port: "<< 5000;

    if(this->writeProtoMessage(1, socketInfoRequest))
    {
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Sent SocketInfoRequest.";
    }
    else
    {
        OPENAUTO_LOG(error) << "[BluetoothAdvertiseService] Error sending SocketInfoRequest.";
    }
}
void BluetoothAdvertiseService::writeSocketInfoResponse()
{
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Sending SocketInfoResponse.";
    QString ipAddr;
    foreach(QHostAddress addr, QNetworkInterface::allAddresses())
    {
        if(!addr.isLoopback() && (addr.protocol() == QAbstractSocket::IPv4Protocol))
        {
            ipAddr = addr.toString();
        }
    }

    extraprotos::SocketInfoResponse socketInfoResponse;
    socketInfoResponse.set_ip_address(ipAddr.toStdString());
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] ipAddress: "<< ipAddr.toStdString();

    socketInfoResponse.set_port(5000);
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] port: "<< 5000;

    socketInfoResponse.set_status(extraprotos::Status::STATUS_SUCCESS);
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] status: "<< extraprotos::Status::STATUS_SUCCESS;


    if(this->writeProtoMessage(7, socketInfoResponse))
    {
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Sent SocketInfoResponse.";
    }
    else
    {
        OPENAUTO_LOG(error) << "[BluetoothAdvertiseService] Error sending SocketInfoResponse.";
    }
}

void BluetoothAdvertiseService::handleSocketInfoRequestResponse(QByteArray data)
{
    extraprotos::SocketInfoResponse socketInfoResponse;
    socketInfoResponse.ParseFromArray(data, data.size());
    OPENAUTO_LOG(info) <<"[BluetoothAdvertiseService] Received SocketInfoRequestResponse, status: "<<socketInfoResponse.status();
    if(socketInfoResponse.status() == 0)
    {
        // A status of 0 should be successful handshake (unless phone later reports an error, aw well)
        // save this phone so we can autoconnect to it next time
        config_->setLastBluetoothPair(socket_->peerAddress().toString().toStdString());
        config_->save();
    }
}


void BluetoothAdvertiseService::handleSocketInfoRequest(QByteArray data)
{
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Reading SocketInfoRequest.";

    extraprotos::SocketInfoRequest socketInfoRequest;

    writeSocketInfoResponse();
}

void BluetoothAdvertiseService::writeNetworkInfoMessage()
{
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Sending NetworkInfoMessage.";

    extraprotos::NetworkInfo networkMessage;
    networkMessage.set_ssid(config_->getWifiSSID());
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] SSID: "<<config_->getWifiSSID();

    networkMessage.set_psk(config_->getWifiPassword());
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] PSKEY: "<<config_->getWifiPassword();

    networkMessage.set_mac_addr(QNetworkInterface::interfaceFromName("wlan0").hardwareAddress().toStdString());
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] MAC: "<<QNetworkInterface::interfaceFromName("wlan0").hardwareAddress().toStdString();

    networkMessage.set_security_mode(extraprotos::SecurityMode::WPA2_PERSONAL);
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Security: "<< extraprotos::SecurityMode::WPA2_PERSONAL;

    networkMessage.set_ap_type(extraprotos::AccessPointType::STATIC);
    OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] AP Type: "<< extraprotos::AccessPointType::STATIC;


    if(this->writeProtoMessage(3, networkMessage))
    {
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Sent NetworkInfoMessage";
    }
    else
    {
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Error sending NetworkInfoMessage.";
    }
}

void BluetoothAdvertiseService::handleUnknownMessage(int messageType, QByteArray data)
{
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Received unknown MessageType of "<<messageType;
        OPENAUTO_LOG(info) << "[BluetoothAdvertiseService] Unknown Message Data: "<<data.toHex(' ').toStdString()  ;
}

void BluetoothAdvertiseService::readSocket()
{
    if(!socket_)
    {
        return;
    }

    auto data = socket_->readAll();
    if(data.length() == 0)
    {
        return;
    }

    uint16_t messageType = (data[2] << 8) | data[3];
    switch(messageType)
    {
        case 1:
            handleSocketInfoRequest(data);
            break;
        case 2:
            writeNetworkInfoMessage();
            break;
        case 7:
            data.remove(0, 4);
            handleSocketInfoRequestResponse(data);
            break;
        default:
            data.remove(0, 4);
            handleUnknownMessage(messageType, data);
            break;
    }
}

void BluetoothAdvertiseService::connectToLastPairedDevice()
{
    QString lastPairedAddress = QString::fromStdString(config_->getLastBluetoothPair());
    if (!lastPairedAddress.isEmpty()) {
        QProcess::execute(QString("bluetoothctl connect %1").arg(lastPairedAddress));
    }
}

}
}
