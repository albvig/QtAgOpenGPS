import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import QtQml.Models
import "components" as Comp

import org.agopengps.bluetooth 1.0

Window{
    id: bluetoothMenu
    title: qsTr("Bluetooth")
    width: 500
    height: 500

    Comp.ButtonColor{
        id: search
        anchors.left: parent.left
        anchors.margins: 10
        anchors.top: parent.top
        height: 50
        onClicked: {
            if(utils.isTrue(settings.setBluetooth_isOn) === false)
                settings.setBluetooth_isOn = true
            agio.bt_search()
        }
        text: qsTr("Search")
        width: 250

        Connections {
            target: agio
            function onSearchingForBluetoothChanged(){
                search.text = agio.searchingForBluetooth ? qsTr("Searching") : qsTr("Search")
            }
        }
        ListView{
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: search.right
            anchors.margins: 10
            model: BluetoothDeviceList{}
            /*delegate: ListDelegate{
                text: model.display
            }
            highlight: ListHighlight {}*/
        }
    }
}
