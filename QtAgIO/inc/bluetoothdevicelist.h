#ifndef BLUETOOTHDEVICELIST_H
#define BLUETOOTHDEVICELIST_H

#include <QtCore>
#include <QtGui>

class BluetoothDeviceList : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit BluetoothDeviceList(QObject *parent = 0);
    ~BluetoothDeviceList();

public:
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

private:
    QList<QString> m_data;

};

#endif // BLUETOOTHDEVICELIST_H
