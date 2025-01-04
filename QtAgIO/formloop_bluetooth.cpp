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
    QStringList deviceList = property_setBluetooth_deviceList.value().toStringList();

    //check if we are already connected
    if(connectedBTDevices.contains(device.name())) return;

    if(deviceList.contains(device.name())){
        qDebug() << "Bluetooth Device found!";
        connectToBluetoothDevice(device);
    }
}

void FormLoop::connectToBluetoothDevice(const QBluetoothDeviceInfo &device){
    QBluetoothAddress address;
    QBluetoothUuid uuid(QStringLiteral("00001101-0000-1000-8000-00805f9b34fb"));

    address = device.address();
    //uuid = device.deviceUuid();

    qDebug() << "Attempting to connect to ";
    qDebug() << device.name();

    bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    connect(bluetoothSocket, &QBluetoothSocket::connected, this, [this, device]() {
        bluetoothConnected(device);  // Pass device to bluetoothConnected
    });
    connect(bluetoothSocket, &QBluetoothSocket::disconnected, this, [this, device]() {
        bluetoothDisconnected(device);  // Optionally pass device to bluetoothDisconnected
    });
    connect(bluetoothSocket, SIGNAL(readyRead()), this, SLOT(readBluetoothData()));
    bluetoothSocket->connectToService(address, uuid, QIODeviceBase::ReadOnly);
}

void FormLoop::bluetoothConnected(const QBluetoothDeviceInfo &device){
    qDebug() << "Connected to bluetooth device " << device.name();
    connectedBTDevices.append(device.name());
    agio->setProperty("connectedBTDevices", connectedBTDevices);
    qDebug() << "Waiting for incoming data...";
}
void FormLoop::bluetoothDisconnected(const QBluetoothDeviceInfo &device){
    TimedMessageBox(2000, "Bluetooth Device Disconnected!", "Disconnected from device " + device.name());
    connectedBTDevices.removeAll(device.name());
    agio->setProperty("connectedBTDevices", connectedBTDevices);
}

void FormLoop::readBluetoothData(){
    QByteArray data = bluetoothSocket->readAll();
    qDebug() << "Received data: " << data;
    //btNMEABuffer += QString::fromLatin1(data);
    btRawBuffer += QString::fromLatin1(data);
    //qDebug() << rawBuffer;
    ParseNMEA(btRawBuffer);
}

void FormLoop::bluetoothDiscoveryFinished(){
    startBluetoothDiscovery();
}
