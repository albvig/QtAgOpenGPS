// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Displays a push button with text, and can be set to any color
import QtQuick
import QtQuick.Controls.Fusion

Button {
    implicitWidth: 120 * theme.scaleWidth
    implicitHeight: 65 * theme.scaleHeight
    id: icon_button
    text: ""
    hoverEnabled: true
    //checkable: true
    icon.source: ""
    icon.color: "transparent"

    property double iconHeightScaleText: 0.75
    property int border: 1

    property color color: "white"

    property color colorHover: "white"

    property color colorChecked: "green"


    //For compatibility with the old IconButton and friends
    property bool isChecked: icon_button.checked
    property string buttonText: ""

    onButtonTextChanged: {
        text = buttonText
    }
    onIsCheckedChanged: {
        checked = isChecked;
    }

    //This is specific to this base type... must be re-implemented in subtypes
    onCheckedChanged: {
        isChecked = checked
    }


    property int radius: 10

    onWidthChanged: {
        //console.warn(text, "Width is now ", width)
    }

    onHeightChanged: {
        //console.warn(height)
    }

    background: Rectangle {
        border.width: icon_button.border
        border.color: icon_button.enabled ? "black" : "grey"
        radius: 10
        id: icon_button_background
        color: icon_button.checked ? icon_button.colorChecked : icon_button.color

    }
}
