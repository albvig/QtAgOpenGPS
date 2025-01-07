import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import QtQml.Models
import "components" as Comp

Window{
    id: bluetoothMenu
    title: qsTr("Bluetooth")
    width: 500
    height: 500

    Comp.TitleFrame{
        id: devicesTitleFrame
        title: qsTr("Connect to Device:")
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.horizontalCenter
        anchors.margins: 10
        border.width: 2
        ListView{
            id: bluetoothDevices
            anchors.fill: parent

            // Custom ListModel to store Bluetooth devices
            model: ListModel {
                id: deviceListModel
            }
            delegate: RadioButton{
                width: bluetoothDevices.width
                id: control

                indicator: Rectangle{
                    anchors.fill: parent
                    color: control.down ? "blue" : devicesTitleFrame.color
                    visible: true
                    anchors.margins: 5
                    border.color: "black"
                    border.width: 1
                    radius: 3
                    Text{
                        text: model.name
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //add the name of device to list of hosts we want to connect to
                            var deviceList = settings.setBluetooth_deviceList

                            var nameStr = model.name;  // name to add

                            // Check if the name is already in the list
                            if (deviceList.indexOf(nameStr) === -1)
                                deviceList.push(nameStr);  // Append if not already present

                            settings.setBluetooth_deviceList = deviceList // save to settings
                            //start the search process
                            agio.bt_search()
                        }
                    }
                }
            }

            Connections {
                target: bluetoothDeviceList

                // When the backend model changes, update the ListModel
                function onModelChanged() {
                    bluetoothDevices.update_list()
                }
            }
            function update_list(){
                // Clear the QML model
                deviceListModel.clear()

                // Populate with updated data
                for (let i = 0; i < bluetoothDeviceList.rowCount(); ++i) {
                    let name = bluetoothDeviceList.get(i); // Fetch name
                    deviceListModel.append({"name": name}); // Append to QML model
                }

            }

            // Initial data load when the component is created
            Component.onCompleted: {
                update_list()
            }

        }
    }
}
