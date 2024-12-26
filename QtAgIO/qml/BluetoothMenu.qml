import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import "components" as Comp

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
        onClicked: agio.bt_search()
        text: qsTr("Search")
        width: 250
    }
}
