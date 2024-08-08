#pragma once

#include <QObject>
#include <QBluetoothServiceInfo>
#include "openauto/Projection/IBluetoothDevice.hpp"
#include <QBluetoothServer>
#include "IService.hpp"
#include "openauto/Configuration/IConfiguration.hpp"
#include <extra_protos/NetworkInfo.pb.h>
#include <extra_protos/SocketInfoRequest.pb.h>
#include <extra_protos/SocketInfoResponse.pb.h>

namespace openauto
{
namespace service
{

class BluetoothAdvertiseService: public QObject
{
    Q_OBJECT

public:
    BluetoothAdvertiseService(configuration::IConfiguration::Pointer configuration);
    void startAdvertising();
    void connectToLastPairedDevice();


private slots:
    void onClientConnected();
    void readSocket();

private:

    projection::IBluetoothDevice::Pointer bluetoothDevice_;

    QBluetoothServiceInfo serviceInfo_;

    std::unique_ptr<QBluetoothServer> server_;
    QBluetoothSocket* socket_;
    configuration::IConfiguration::Pointer config_;

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
