import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var activeController
    readonly property bool isTrashView: !!(root.activeController && root.activeController.currentPath
                                           && root.activeController.currentPath.startsWith("trash:"))
    readonly property int selectedCount: (root.activeController && root.activeController.selectedPaths
                                          && root.activeController.selectedPaths.length !== undefined)
                                         ? root.activeController.selectedPaths.length : 0

    function formatSize(bytes) {
        var value = Number(bytes || 0)
        var units = ["B", "KB", "MB", "GB", "TB"]
        var unitIndex = 0
        while (value >= 1024 && unitIndex < units.length - 1) {
            value /= 1024
            unitIndex += 1
        }
        return (unitIndex === 0 ? value.toFixed(0) : value.toFixed(value >= 10 ? 1 : 2)) + " " + units[unitIndex]
    }

    onActiveControllerChanged: {
        if (root.isTrashView && root.activeController && root.activeController.requestTrashMetrics) {
            root.activeController.requestTrashMetrics()
        }
    }

    Connections {
        target: root.activeController
        function onCurrentPathChanged() {
            if (root.isTrashView && root.activeController && root.activeController.requestTrashMetrics) {
                root.activeController.requestTrashMetrics()
            }
        }
    }

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

        Text {
            visible: selectedCount > 0
            text: qsTr("%1 items selected").arg(selectedCount)
            color: theme.accent
            font.pixelSize: 11
            font.bold: true
        }

        Rectangle {
            visible: !!(root.activeController && root.activeController.hasClipboard)
            radius: theme.rSm
            color: theme.surface
            border.color: theme.border
            border.width: 1
            implicitHeight: 26
            implicitWidth: clipboardRow.implicitWidth + 20

            RowLayout {
                id: clipboardRow
                anchors.centerIn: parent
                spacing: theme ? theme.space6 : 6

                Text {
                    text: root.activeController && root.activeController.isCutOp ? "Cut" : "Copied"
                    color: root.activeController && root.activeController.isCutOp ? "#C9645A" : theme.accent
                    font.pixelSize: 11
                    font.bold: true
                }

                Text {
                    text: qsTr("%1 item(s)").arg(root.activeController ? root.activeController.clipboardItemCount : 0)
                    color: theme.textPrimary
                    font.pixelSize: 11
                }

                Text {
                    text: root.activeController
                        ? (root.activeController.clipboardSizePending ? "Calculating size..." : root.formatSize(root.activeController.clipboardTotalSize))
                        : "0 B"
                    color: theme.textSecondary
                    font.pixelSize: 11
                }

                Text {
                    text: root.activeController ? root.activeController.clipboardPreview : ""
                    color: theme.textMuted
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }
        }

        Rectangle {
            visible: root.isTrashView
            radius: theme.rSm
            color: theme.surface
            border.color: theme.border
            border.width: 1
            implicitHeight: 26
            implicitWidth: trashRow.implicitWidth + 20

            RowLayout {
                id: trashRow
                anchors.centerIn: parent
                spacing: theme ? theme.space6 : 6

                Text {
                    text: "Trash"
                    color: theme.textSecondary
                    font.pixelSize: 11
                    font.bold: true
                }

                Rectangle {
                    width: 1
                    height: 12
                    color: theme.border
                }

                Text {
                    text: qsTr("%1 items").arg(root.activeController ? root.activeController.trashItemCount : 0)
                    color: theme.textPrimary
                    font.pixelSize: 11
                }

                Text {
                    text: root.formatSize(root.activeController ? root.activeController.trashSize : 0)
                    color: theme.textSecondary
                    font.pixelSize: 11
                }
            }
        }

        Button {
            visible: selectedCount > 0
            text: "Clear"
            flat: true
            font.pixelSize: 10
            onClicked: if (root.activeController) root.activeController.clearSelection()
            background: null
            contentItem: Text { text: parent.text; color: theme.textMuted; font.bold: true }
        }

        Button {
            visible: !!(root.activeController && root.activeController.hasClipboard)
            text: "Clear Clipboard"
            flat: true
            font.pixelSize: 10
            onClicked: if (root.activeController) root.activeController.clearClipboard()
            background: null
            contentItem: Text { text: parent.text; color: theme.textMuted; font.bold: true }
        }

        Button {
            visible: root.isTrashView
            text: "Empty Trash"
            flat: true
            font.pixelSize: 10
            onClicked: if (root.activeController) appWindow.requestEmptyTrash(root.activeController)
            background: null
            contentItem: Text { text: parent.text; color: theme.accent; font.bold: true }
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
