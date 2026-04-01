import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import liphis

Dialog {
    id: root
    title: "Disk Usage Analysis"
    width: 640
    height: 720
    modal: true
    closePolicy: Popup.CloseOnEscape
    anchors.centerIn: parent
    standardButtons: Dialog.NoButton
    
    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }
    property var analysisController: null
    property var toastManager: null

    property string currentItem: ""
    property var liveSize: 0
    property int liveFiles: 0
    property int liveDirs: 0
    property bool isScanning: false

    function resetDialog() {
        listModel.clear()
        liveSize = 0
        liveFiles = 0
        liveDirs = 0
        currentItem = ""
        isScanning = true
        open()
    }

    function addNode(node) {
        listModel.append(node)
    }

    function setProgress(size, files, dirs, item) {
        liveSize = size
        liveFiles = files
        liveDirs = dirs
        currentItem = item
    }

    function finish(result) {
        var listedTotal = 0
        for (var i = 0; i < listModel.count; ++i) {
            var node = listModel.get(i)
            listedTotal += (node && node.size !== undefined) ? Number(node.size) : 0
        }
        if (result && result.size !== undefined) {
            liveSize = Math.max(Number(result.size), listedTotal)
        } else {
            liveSize = listedTotal
        }
        if (result && result.fileCount !== undefined) {
            liveFiles = result.fileCount
        }
        isScanning = false
    }

    function formatSize(bytes) {
        if (bytes < 1024) return bytes + " B"
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + " KB"
        if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(1) + " MB"
        return (bytes / (1024 * 1024 * 1024)).toFixed(2) + " GB"
    }

    ListModel { id: listModel }

    background: Rectangle { 
        color: theme.surfaceMain
        radius: theme.radiusLarge
        border.color: theme.border
        layer.enabled: true
        layer.effect: DropShadow { radius: 10; color: Qt.rgba(0, 0, 0, theme.isDark ? 0.18 : 0.10); samples: 16; verticalOffset: 2 }
    }

    ColumnLayout {
        anchors.fill: parent; anchors.margins: theme.spacingLarge; spacing: theme.spacingMedium

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true; spacing: 4
                Text { 
                    text: isScanning ? "Analysing System..." : "Analysis Complete"
                    font.bold: true; font.pixelSize: 20; color: theme.textPrimary
                }
                Text {
                    text: liveFiles + " files, " + liveDirs + " folders scanned"
                    font.pixelSize: 12; color: theme.textSecondary
                }
            }
            ToolButton { 
                Layout.preferredWidth: 36; Layout.preferredHeight: 36; flat: true
                onClicked: {
                    if (isScanning && analysisController) analysisController.cancel()
                    root.close()
                }
                contentItem: Icon { name: "close"; color: theme.textSecondary; anchors.centerIn: parent; iconSize: 20 }
                background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
            }
        }

        // Progress Card
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 100; color: theme.surfaceElevated; radius: theme.radius; border.color: theme.border
            ColumnLayout {
                anchors.fill: parent; anchors.margins: theme.spacingMedium; spacing: theme.spacingSmall
                RowLayout {
                    Text { text: "TOTAL SIZE"; font.pixelSize: 10; color: theme.textMuted; font.letterSpacing: 1; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Text { text: formatSize(root.liveSize); font.pixelSize: 16; font.bold: true; color: theme.accent }
                }
                ProgressBar { 
                    Layout.fillWidth: true; indeterminate: isScanning; value: isScanning ? 0 : 1;
                    contentItem: Item {
                        Rectangle {
                            width: parent.width * parent.parent.value; height: 6; radius: 3
                            color: theme.accent
                        }
                    }
                    background: Rectangle { height: 6; radius: 3; color: theme.surfaceSecondary }
                }
                RowLayout {
                    visible: isScanning; spacing: 6
                    Icon { name: "info"; color: theme.textMuted; iconSize: 12 }
                    Text { 
                        text: root.currentItem; font.pixelSize: 10; color: theme.textMuted; elide: Text.ElideMiddle; Layout.fillWidth: true 
                    }
                }
            }
        }

        ListView {
            id: listView
            Layout.fillWidth: true; Layout.fillHeight: true; clip: true
            model: listModel; spacing: 8
            delegate: Rectangle {
                width: listView.width; height: 48; radius: theme.radiusSmall; color: theme.surfaceElevated; border.color: theme.border
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12; spacing: 12
                    FileIcon { Layout.preferredWidth: 20; Layout.preferredHeight: 20; isDir: model.isDir; theme: theme }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 4
                        Text { text: model.name; elide: Text.ElideRight; font.pixelSize: 12; color: theme.textPrimary }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 4; color: theme.surfaceSecondary; radius: 2
                            Rectangle {
                                width: parent.width * (model.size / Math.max(1, root.liveSize)); height: 4; color: theme.accent; radius: 2; opacity: 0.8
                            }
                        }
                    }
                    Text { text: formatSize(model.size); font.pixelSize: 12; font.bold: true; color: theme.textPrimary }
                }
            }
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }

        RowLayout {
            Layout.fillWidth: true; spacing: theme.spacingMedium
            Button {
                text: "COPY TREE"
                Layout.fillWidth: true; Layout.preferredHeight: 40
                enabled: !isScanning
                onClicked: {
                    if (analysisController) analysisController.copyTreeToClipboard()
                    if (toastManager) toastManager.show("Tree copied to clipboard!")
                }
                background: Rectangle { radius: theme.radiusSmall; border.color: theme.borderStrong; color: parent.hovered ? theme.hover : "transparent"; opacity: parent.enabled ? 1 : 0.4 }
                contentItem: Text { text: parent.text; color: theme.textPrimary; font.bold: true; font.pixelSize: 12; font.letterSpacing: 1; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
            Button {
                text: isScanning ? "STOP" : "CLOSE"
                Layout.fillWidth: true; Layout.preferredHeight: 40
                onClicked: {
                    if (isScanning && analysisController) analysisController.cancel()
                    else root.close()
                }
                background: Rectangle { 
                    radius: theme.radiusSmall
                    color: parent.hovered ? theme.surfaceElevated : theme.surfaceSecondary
                    border.color: theme.border
                }
                contentItem: Text { text: parent.text; color: theme.textPrimary; font.bold: true; font.pixelSize: 12; font.letterSpacing: 1; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
        }
    }
}
