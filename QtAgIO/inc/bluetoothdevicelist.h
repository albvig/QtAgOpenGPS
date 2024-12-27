#ifndef BLUETOOTHDEVICELIST_H
#define BLUETOOTHDEVICELIST_H

#include <QtCore>
#include <QtGui>
#include <QHash>

class BluetoothDeviceList : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit BluetoothDeviceList(QObject *parent = 0);
    ~BluetoothDeviceList();

    Q_INVOKABLE void addDevice(const QString &deviceName, const QString &deviceID);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

private:
    QList<QString> m_data;

};

#endif // BLUETOOTHDEVICELIST_H
