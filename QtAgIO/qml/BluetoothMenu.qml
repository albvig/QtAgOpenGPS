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
    }

    Comp.TitleFrame{
        id: devicesTitleFrame
        title: qsTr("Devices")
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: search.right
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
                            if (deviceList.indexOf(nameStr) === -1) {
                                deviceList.push(nameStr);  // Append if not already present
                                console.log("Added: " + nameStr);
                            } else {
                                console.log(nameStr + " is already in the list.");
                            }

                            console.log("Current device list: " + deviceList);

                            settings.setBluetooth_deviceList = deviceList
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
                console.log("Model changed!")

                // Clear the QML model
                deviceListModel.clear()

                // Populate with updated data
                for (let i = 0; i < bluetoothDeviceList.rowCount(); ++i) {
                    let name = bluetoothDeviceList.get(i); // Fetch name
                    deviceListModel.append({"name": name}); // Append to QML model
                    console.log(name);
                }

            }

            // Initial data load when the component is created
            Component.onCompleted: {
                update_list()
            }

        }
    }
}
