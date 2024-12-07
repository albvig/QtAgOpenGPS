// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
//
import QtQuick
import QtQuick.Layouts
import Qt.labs.folderlistmodel
import QtQuick.Controls.Fusion
import QtQuick.Controls.Material

import ".."
import "./components"


Item {

    id: mainWindowTram
    width: parent.width
    height: parent.height


    Popup {
        //anchors.left: parent.left
        //anchors.bottom: parent.bottom
        //anchors.right: parent.right
        //anchors.top: topLine.bottom
        height: 500  * theme.scaleHeight
        width: 250  * theme.scaleWidth
        modal: true
        visible: mainWindowTram.visible
        anchors.centerIn: parent
        TopLine{
            id: tramtopLine
            titleText: "Tram Lines"
        }
        Row{
            anchors.top: tramtopline.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            id: tramHalfNudge
            width: children.width
            spacing: 35
            height: children.height
            IconButtonTransparent{
                icon.source: prefix + "/images/SnapLeftHalf.png"
            }
            Text{
                text: "2.00 m"
                font.bold: true
                font.pixelSize: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButtonTransparent{
                icon.source: prefix + "/images/SnapRightHalf.png"
            }
        }
        Row{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: tramHalfNudge.bottom
            anchors.topMargin: 30
            id: tramSmallNudge
            width: children.width
            spacing: 25
            height: children.height
            IconButtonTransparent{
                icon.source: prefix + "/images/ArrowLeft.png"
            }
            Text{
                text: "10 cm"
                font.bold: true
                font.pixelSize: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButtonTransparent{
                icon.source: prefix + "/images/ArrowRight.png"
            }
        }
        IconButtonTransparent{
            id: tramSwapAB
            anchors.top: tramSmallNudge.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 60
            icon.source: prefix + "/images/ABSwapPoints.png"
        }
        IconButtonTransparent{
            id: tramSwapMode
            anchors.top: tramSmallNudge.bottom
            anchors.topMargin: 10
            anchors.left: tramSwapAB.right
            anchors.leftMargin: 60
            icon.source: prefix + "/images/TramLines.png"
        }
        Text {
            text: qsTr("Mode")
            font.pixelSize: 15
            anchors.left: tramSwapMode.right
            anchors.leftMargin: 30
            anchors.verticalCenter: tramSwapMode.verticalCenter
        }
        Row{
            id: tramPasses
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: tramSwapMode.bottom
            anchors.topMargin: 30
            height:tramPassesDown.height
            spacing: 25
            IconButtonTransparent{
                id: tramPassesDown
                icon.source: prefix + "/images/DnArrow64.png"
            }
            SpinBoxCustomized{
                id: passesRow
                width: tramPassesDown.width
                height: tramPassesDown.height
                text: qsTr("Passes")
                from: 1
                value: 1
                to: 999
            }
            IconButtonTransparent{
                icon.source: prefix + "/images/UpArrow64.png"
            }
        }

        IconButtonTransparent{
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            icon.source: prefix + "/images/SwitchOff.png"
            onClicked: tramLinesEditor.visible = false
        }
        IconButtonTransparent{
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            icon.source: prefix + "/images/VehFileSave.png"
        }
        Column{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 30
            anchors.top: tramPasses.bottom
            spacing: 10
            Text{
                text: "Seed"
            }
            Text{
                text: "Spray"
            }
            Text{
                text: "Track"
            }
        }
    }
}
