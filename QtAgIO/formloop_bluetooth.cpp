#include "formloop.h"
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include "bluetoothdevicelist.h"
#include "properties.h"

void FormLoop::startBluetoothDiscovery(){
    // Create a discovery agent and connect to its signals
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(bluetoothDeviceDiscovered(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(bluetoothDiscoveryFinished()));

    // Start a discovery
    discoveryAgent->start();
    agio->setProperty("searchingForBluetooth", true);
}
// In your local slot, read information about the found devices
void FormLoop::bluetoothDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    devList->addDevice(device.name());
    QStringList deviceList = property_setBluetooth_deviceList.value().toStringList();
    if(deviceList.contains(device.name())){// hard coded for testing
        qDebug() << "Bluetooth Device found!";
        connectToBluetoothDevice(device);
    }
}

void FormLoop::connectToBluetoothDevice(const QBluetoothDeviceInfo &device){
    QBluetoothAddress address;
    QBluetoothUuid uuid(QStringLiteral("00001101-0000-1000-8000-00805f9b34fb"));

    address = device.address();
    //uuid = device.deviceUuid();

    //this section adds every device we try to connect to to a list that we automatically try to
    //connect to every time
    QString nameStr = device.name(); // Convert address to string

    //create a stringlist to hold the property
    QStringList deviceList = property_setBluetooth_deviceList.value().toStringList();
    // Check if address is already in the property list
    if (!deviceList.contains(nameStr)) {
        deviceList.append(nameStr); // Add if not present
        //set the property to the stringlist
        property_setBluetooth_deviceList = deviceList;
    }
    qDebug() << "Attempting to connect to ";
    qDebug() << device.name();

    bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    connect(bluetoothSocket, SIGNAL(connected()), this, SLOT(bluetoothConnected()));

    connect(bluetoothSocket, SIGNAL(readyRead()), this, SLOT(readBluetoothData()));
    bluetoothSocket->connectToService(address, uuid, QIODeviceBase::ReadOnly);
}

void FormLoop::bluetoothConnected(){
    qDebug() << "Connected to bluetooth device!";
    qDebug() << "Waiting for incoming data...";
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
    agio->setProperty("searchingForBluetooth", false);
    qDebug() << "bt disc finished";
    startBluetoothDiscovery();
}
