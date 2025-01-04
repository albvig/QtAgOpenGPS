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

    Q_INVOKABLE void addDevice(const QString &deviceName);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    QList<QString> m_data;
    Q_INVOKABLE QString get(int row) const;
signals:
    void modelChanged();

};

#endif // BLUETOOTHDEVICELIST_H
