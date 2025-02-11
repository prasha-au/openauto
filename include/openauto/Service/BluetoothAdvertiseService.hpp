#pragma once

#include <QObject>
#include <QBluetoothServiceInfo>
#include "openauto/Projection/IBluetoothDevice.hpp"
#include <QBluetoothServer>
#include "IService.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include "aasdk_proto/NetworkInfo.pb.h"
#include "aasdk_proto/SocketInfoRequest.pb.h"
#include "aasdk_proto/SocketInfoResponse.pb.h"

namespace openauto
{
namespace service
{

class BluetoothAdvertiseService: public QObject
{
    Q_OBJECT

public:
    BluetoothAdvertiseService(configuration::Configuration::Pointer configuration);
    void startAdvertising();


private slots:
    void onClientConnected();
    void readSocket();

private:

    projection::IBluetoothDevice::Pointer bluetoothDevice_;

    QBluetoothServiceInfo serviceInfo_;

    std::unique_ptr<QBluetoothServer> server_;
    QBluetoothSocket* socket_;
    configuration::Configuration::Pointer config_;

    void handleUnknownMessage(int messageType, QByteArray data);
    void handleSocketInfoRequest(QByteArray data);
    void handleSocketInfoRequestResponse(QByteArray data);
    void writeSocketInfoRequest();
    void writeSocketInfoResponse();
    void writeNetworkInfoMessage();
    bool writeProtoMessage(uint16_t messageType, google::protobuf::Message& message);

};


}
}
