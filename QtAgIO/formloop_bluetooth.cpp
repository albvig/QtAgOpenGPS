#include "formloop.h"
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>


using namespace Qt::StringLiterals;

static constexpr auto serviceUuid = "e8e10f95-1a70-4b27-9ccf-02010264e9c8"_L1;

void FormLoop::startBluetoothDiscovery(){
    // Create a discovery agent and connect to its signals
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(bluetoothDeviceDiscovered(QBluetoothDeviceInfo)));

    // Start a discovery
    discoveryAgent->start();
    qDebug() << "discovery started";
}
// In your local slot, read information about the found devices
void FormLoop::bluetoothDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
    if(device.name() == "RTK_GNSS_305"){// hard coded for testing
        qDebug() << "RTK Device found!";
        connectToBluetoothDevice(device);
    }
}

void FormLoop::connectToBluetoothDevice(const QBluetoothDeviceInfo &device){
    QBluetoothAddress address;
    QBluetoothUuid uuid(QStringLiteral("00001101-0000-1000-8000-00805f9b34fb"));

    address = device.address();
    //uuid = device.deviceUuid();

    qDebug() << "Attempting to connect to ";
    qDebug() << address << " " << uuid;
    QBluetoothSocket *bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    connect(bluetoothSocket, SIGNAL(connected()), this, SLOT(bluetoothConnected()));
    bluetoothSocket->connectToService(address, uuid, QIODeviceBase::ReadOnly);
}

void FormLoop::bluetoothConnected(){
    qDebug() << "Connected to bluetooth device!";
}
