import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: 400
    height: 180
    anchors.centerIn: parent
    
    property string statusText: "Processing..."
    property double progress: 0.0
    property var theme

    background: Rectangle {
        color: theme.surfaceRaised
        radius: 12
        border.color: theme.border
    }

    contentItem: ColumnLayout {
        spacing: 20
        anchors.margins: 24

        Text {
            text: root.statusText
            color: theme.textPrimary
            font.pixelSize: 14
            font.bold: true
            Layout.fillWidth: true
        }

        ProgressBar {
            id: progressBar
            value: root.progress
            Layout.fillWidth: true
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 6
                color: theme.surfaceMuted
                radius: 3
            }
            contentItem: Item {
                implicitWidth: 200
                implicitHeight: 6
                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    radius: 3
                    color: theme.accent
                }
            }
        }

        Text {
            text: Math.round(root.progress * 100) + "%"
            color: theme.textSecondary
            font.pixelSize: 12
            Layout.alignment: Qt.AlignRight
        }
    }
}
