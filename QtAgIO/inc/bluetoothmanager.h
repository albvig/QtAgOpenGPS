#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include "formloop.h"


class BluetoothManager : public QObject
{
    Q_OBJECT
private:
    FormLoop* formLoop;

public:
    explicit BluetoothManager(FormLoop* loop, QObject* parent = nullptr);    ~BluetoothManager();
public slots:
    void startBluetoothDiscovery();

private:
    QBluetoothSocket *socket;

    QString connectedDeviceName;
    QString rawBuffer;

    bool deviceConnected = false;
    bool deviceConnecting = false;

private slots:
    void connectToDevice(const QBluetoothDeviceInfo &device);
    void connected();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void disconnected();
    void discoveryFinished();
    void onSocketErrorOccurred(QBluetoothSocket::SocketError error);
    void readData();
};
#endif  //BLUETOOTHMANAGER_H
