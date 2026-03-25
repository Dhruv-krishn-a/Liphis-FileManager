import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var activeController

    height: theme.statusBarHeight
    color: theme.surfaceRaised
    border.color: theme.border

    Rectangle {
        anchors.top: parent.top
        width: parent.width
        height: 1
        color: theme.border
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: theme ? theme.space16 : 16
        anchors.rightMargin: theme ? theme.space16 : 16
        spacing: theme ? theme.space8 : 8

        Text {
            text: (root.activeController && root.activeController.loading) ? "Scanning..." : "Ready"
            color: theme.textSecondary
            font.pixelSize: theme ? theme.fontBody - 1 : 11
        }

        Item { Layout.fillWidth: true }

        Row {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            spacing: theme ? theme.space6 : 6

            Repeater {
                model: (root.activeController && root.activeController.availableExtensions)
                    ? root.activeController.availableExtensions
                    : []

                delegate: FilterChip {
                    theme: root.theme
                    text: modelData
                    selected: !!(root.activeController && root.activeController.fileModel
                        && root.activeController.fileModel.extensionFilter === modelData)
                    onClicked: {
                        if (!root.activeController || !root.activeController.fileModel) return
                        root.activeController.fileModel.extensionFilter = selected ? "" : modelData
                    }
                }
            }
        }
    }
}
