#include "bluetoothdevicelist.h"

BluetoothDeviceList::BluetoothDeviceList(QObject *parent)
    : QAbstractListModel(parent)
{

}

BluetoothDeviceList::~BluetoothDeviceList()
{

}

int BluetoothDeviceList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_data.count();
}


QVariant BluetoothDeviceList::data(const QModelIndex &index, int role) const
{
    // the index returns the requested row and column information.
    // we ignore the column and only use the row information
    int row = index.row();

    // boundary check for the row
    if(row < 0 || row >= m_data.count()) {
        return QVariant();
    }

    const DeviceInfo &device = m_data.at(row);
    switch(role) {
    case Qt::DisplayRole:
        return QString("%1 (%2)").arg(device.name, device.id);
    case Qt::UserRole:
        return device.id;
    }

    // The view asked for other data, just return an empty QVariant
    return QVariant();
}

void BluetoothDeviceList::addDevice(const QString &deviceName, const QString &deviceID){
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append({deviceName, deviceID});
    endInsertRows();
}
QHash<int, QByteArray> BluetoothDeviceList::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display"; // Map DisplayRole to "display"
    roles[Qt::UserRole] = "id";
    return roles;
}

