import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RadioDelegate {
    id: control
    text: model.name
    checked: model.index === trk.idx

    contentItem: RowLayout {
        width: 300

        Image {
            id: trackType
            width: 50
            height: 50
            anchors.margins: 2

            source: (model.mode === 2) ? prefix + "/images/TrackLine.png" :
                    (model.mode === 4) ? prefix + "/images/TrackCurve.png" :
                    (model.mode === 64) ? prefix + "/images/TrackPivot.png" :
                                       prefix + "/images/HelpSmall.png"
        }

        Text {
            id: trackname
            width: 250

            //rightPadding: control.indicator.width + control.spacing
            text: control.text
            font: control.font
            opacity: enabled ? 1.0 : 0.3
            //color: control.down ? "red" : "blue"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        Button {
            id: isTrackVisible
            background: Rectangle {
                implicitWidth: trackname.height
                implicitHeight: trackname.height
                color: model.isVisible ? "green" : "red"
            }
            onClicked: {
                tracksInterface.setVisible(model.index, !model.isVisible)
                model.isVisible = !model.isVisible
            }
        }
    }

    indicator: Rectangle {
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        visible: control.down || control.highlighted || control.checked
        color: control.checked ? aog.blackDayWhiteNight : aog.backgroundColor
    }
}

