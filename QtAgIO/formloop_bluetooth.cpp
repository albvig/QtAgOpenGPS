#include "formloop.h"
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include "bluetoothdevicelist.h"
#include "properties.h"

void FormLoop::startBluetoothDiscovery(){

    //empty device list
    btDevicesList->clear();

    // Create a discovery agent and connect to its signals
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(bluetoothDeviceDiscovered(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(bluetoothDiscoveryFinished()));

    // Start a discovery
    discoveryAgent->start();
}
// In your local slot, read information about the found devices
void FormLoop::bluetoothDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    btDevicesList->addDevice(device.name());

    //check if we are already connected or connecting
    if(bluetoothDeviceConnected || bluetoothDeviceConnecting) return;

    QStringList deviceList = property_setBluetooth_deviceList.value().toStringList();

    if(deviceList.contains(device.name())){
        qDebug() << "Bluetooth Device found!";
        connectToBluetoothDevice(device);
    }
}

void FormLoop::connectToBluetoothDevice(const QBluetoothDeviceInfo &device){
    QBluetoothAddress address;
    QBluetoothUuid uuid(QStringLiteral("00001101-0000-1000-8000-00805f9b34fb"));

    address = device.address();

    //set here as it is used for display purposes. If it actually doesn't connect,
    // it's cleared
    btConnectedDeviceName = device.name();

    qDebug() << "Attempting to connect to ";
    qDebug() << btConnectedDeviceName;

    bluetoothDeviceConnecting = true;

    bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    connect(bluetoothSocket, SIGNAL(errorOccurred(QBluetoothSocket::SocketError)),
            this, SLOT(onSocketErrorOccurred(QBluetoothSocket::SocketError)));
    connect(bluetoothSocket, SIGNAL(connected()), this, SLOT(bluetoothConnected()));
    connect(bluetoothSocket, SIGNAL(disconnected()), this, SLOT(bluetoothDisconnected()));
    connect(bluetoothSocket, SIGNAL(readyRead()), this, SLOT(readBluetoothData()));

    bluetoothSocket->connectToService(address, uuid, QIODeviceBase::ReadOnly);
}

void FormLoop::bluetoothConnected(){
    qDebug() << "Connected to bluetooth device " << btConnectedDeviceName;
    bluetoothDeviceConnecting = false;
    bluetoothDeviceConnected = true;
    agio->setProperty("connectedBTDevices", btConnectedDeviceName);
    qDebug() << "Waiting for incoming data...";
}
void FormLoop::bluetoothDisconnected(){
    TimedMessageBox(2000, "Bluetooth Device Disconnected!", "Disconnected from device " + btConnectedDeviceName);
    bluetoothDeviceConnected = false;
    btConnectedDeviceName.clear();
    agio->setProperty("connectedBTDevices", ""); // tell the frontend nothing is connected
}

void FormLoop::readBluetoothData(){
    QByteArray data = bluetoothSocket->readAll();
    qDebug() << "Received data: " << data;
    btRawBuffer += QString::fromLatin1(data);
    ParseNMEA(btRawBuffer);
}

void FormLoop::bluetoothDiscoveryFinished(){
    startBluetoothDiscovery();
}
void FormLoop::onSocketErrorOccurred(QBluetoothSocket::SocketError error) {
    // Handle different error cases
    switch (error) {
    case QBluetoothSocket::SocketError::HostNotFoundError:
        qDebug() << "Error: Host not found for device" << btConnectedDeviceName;
        break;
    case QBluetoothSocket::SocketError::ServiceNotFoundError:
        qDebug() << "Error: Service not found for device" << btConnectedDeviceName;
        break;
    case QBluetoothSocket::SocketError::NetworkError:
        qDebug() << "Error: Network error occurred while communicating with" << btConnectedDeviceName;
        break;
    case QBluetoothSocket::SocketError::OperationError:
        qDebug() << "Error: Operation error occurred for device" << btConnectedDeviceName;
        break;
    default:
        qDebug() << "Error: Unknown error occurred for device" << btConnectedDeviceName << "Error code:" << error;
    }
}
