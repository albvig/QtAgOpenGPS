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

    // A model can return data for different roles.
    // The default role is the display role.
    // it can be accesses in QML with "model.display"
    switch(role) {
    case Qt::DisplayRole:
        // Return the color name for the particular row
        // Qt automatically converts it to the QVariant type
        return m_data.value(row);
    }

    // The view asked for other data, just return an empty QVariant
    return QVariant();
}

void BluetoothDeviceList::addDevice(const QString &deviceName, const QString &deviceID){
    m_data.append(deviceName);
}
