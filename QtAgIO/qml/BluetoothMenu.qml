import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import "components" as Comp

Window{
    id: bluetoothMenu
    title: qsTr("Bluetooth")

    Comp.ButtonColor{
        id: search
        onClicked: {
            console.log("clk")
            agio.bt_search()
        }
    }
}
