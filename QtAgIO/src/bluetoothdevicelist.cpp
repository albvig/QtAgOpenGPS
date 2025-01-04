#include "bluetoothdevicelist.h"
#include "formloop.h"
#include "agioproperty.h"

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

// Get device name at specific row
QString BluetoothDeviceList::get(int row) const
{
    if (row < 0 || row >= m_data.count())
        return QString();
    return m_data.at(row);
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

    switch(role) {
    case Qt::DisplayRole:
        return m_data.value(row);
    }

    // The view asked for other data, just return an empty QVariant
    return QVariant();
}

void BluetoothDeviceList::addDevice(const QString &deviceName){
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append(deviceName);
    endInsertRows();

    emit modelChanged();
}

void BluetoothDeviceList::clear(){
    m_data.clear();
    emit modelChanged();
}

