#include "formloop.h"
#include <QBluetoothDeviceDiscoveryAgent>

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
}
