// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Dimensions for front 3pt mounted implement
import QtQuick
import QtQuick.Controls.Fusion

import ".."
import "../components"

Rectangle{
    id: configImpDimWin
    anchors.fill: parent
    color: aog.backgroundColor
    visible: false
    Image{
        id: image1
        source: prefix + "/images/ToolHitchPageFront.png"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
		anchors.leftMargin: 7  * theme.scaleWidth
		anchors.rightMargin: 7 * theme.scaleWidth
		anchors.topMargin: 7 * theme.scaleHeight
		anchors.bottomMargin: 7 * theme.scaleHeight
        height: parent.height*.75
    }
    SpinBoxCM{
        id: frontHitchLength
        anchors.top: image1.bottom
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.45
        from: 10
        to:3000
        boundValue: settings.setVehicle_hitchLength < 0 ? -settings.setVehicle_hitchLength : settings.setVehicle_hitchLength
        onValueModified: settings.setVehicle_hitchLength = value
        TextLine{
            text: qsTr("Units: ")+ utils.cm_unit_abbrev()
            font.bold: true
            anchors.top: parent.bottom
        }
    }

}
