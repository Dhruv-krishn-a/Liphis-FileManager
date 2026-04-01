import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import liphis

Dialog {
    id: root
    title: qsTr("Properties: %1").arg(fileName || "")
    modal: true
    closePolicy: Popup.CloseOnEscape
    visible: false
    width: 600
    height: 680
    standardButtons: Dialog.NoButton
    anchors.centerIn: parent

    signal openRequested(string path)
    signal revealRequested(string path)
    signal copyPathRequested(string path)
    signal changePermissionsRequested(string path, string newPerms)

    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }

    property string fileName: ""
    property string filePath: ""
    property string filePerms: ""
    property string fileOwner: ""
    property string fileGroup: ""
    property int fileSizeBytes: 0
    property string fileDateRaw: ""
    property bool isDir: false
    property string mimeType: ""
    property var associatedApps: []
    property var activeController: null

    property var permModel: ({
        owner: { r: false, w: false, x: false },
        group: { r: false, w: false, x: false },
        others: { r: false, w: false, x: false }
    })

    function show(meta) {
        if (!meta) return
        fileName = meta.name || ""
        filePath = meta.path || ""
        filePerms = meta.permissions || ""
        fileOwner = meta.owner || ""
        fileGroup = meta.group || ""
        isDir = !!meta.isDir
        fileSizeBytes = meta.isDir ? 0 : (meta.size || 0)
        fileDateRaw = meta.mtime || meta.date || ""
        mimeType = meta.mimeType || ""
        if (activeController) {
            associatedApps = activeController.getAssociatedApps(filePath)
        }
        parsePermString(filePerms)
        open()
    }

    function humanReadableSize(bytes) {
        if (bytes === 0) return "0 B"
        var units = ["B", "KB", "MB", "GB", "TB"]
        var i = 0; var val = bytes
        while (val >= 1024 && i < units.length - 1) { val /= 1024; i++ }
        return val.toFixed(1) + " " + units[i]
    }

    function formatDate(raw) {
        if (!raw) return "--"
        var d = new Date(raw * 1000)
        return d.toLocaleString()
    }

    function permsToNumeric(model) {
        function tri(o) { return (o.r ? 4 : 0) + (o.w ? 2 : 0) + (o.x ? 1 : 0) }
        return "" + tri(model.owner) + tri(model.group) + tri(model.others)
    }

    function parsePermString(perms) {
        permModel = { owner: { r: false, w: false, x: false }, group: { r: false, w: false, x: false }, others: { r: false, w: false, x: false } }
        if (!perms || perms.length < 9) return
        var s = perms.replace(/[^rwx-]/g, "")
        function sliceTrip(i) {
            var trip = s.substr(i*3, 3)
            return { r: trip[0] === "r", w: trip[1] === "w", x: trip[2] === "x" }
        }
        permModel.owner = sliceTrip(0); permModel.group = sliceTrip(1); permModel.others = sliceTrip(2)
    }

    background: Rectangle {
        color: theme.surfaceElevated
        radius: 16
        border.color: theme.border
        layer.enabled: true
        layer.effect: DropShadow { radius: 10; color: Qt.rgba(0, 0, 0, theme.isDark ? 0.16 : 0.09); samples: 16; verticalOffset: 2 }
    }

    ColumnLayout {
        anchors.fill: parent; anchors.margins: 24; spacing: 20

        RowLayout {
            spacing: 15
            FileIcon { Layout.preferredWidth: 48; Layout.preferredHeight: 48; isDir: root.isDir; theme: theme }
            ColumnLayout {
                Text { text: root.fileName; color: theme.textPrimary; font.pixelSize: 18; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
                Text { text: root.isDir ? "Folder" : "File"; color: theme.textSecondary; font.pixelSize: 12 }
            }
        }

        TabBar {
            id: tabBar; Layout.fillWidth: true; background: null
            TabButton { 
                id: tabGen
                text: "GENERAL"; font.pixelSize: 11; font.letterSpacing: 1; font.bold: true
                contentItem: Text { text: tabGen.text; color: tabGen.checked ? theme.accent : theme.textSecondary; horizontalAlignment: Text.AlignHCenter }
                background: Rectangle { color: "transparent"; Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 2; color: theme.accent; visible: tabGen.checked } }
            }
            TabButton { 
                id: tabPerms
                text: "PERMISSIONS"; font.pixelSize: 11; font.letterSpacing: 1; font.bold: true
                contentItem: Text { text: tabPerms.text; color: tabPerms.checked ? theme.accent : theme.textSecondary; horizontalAlignment: Text.AlignHCenter }
                background: Rectangle { color: "transparent"; Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 2; color: theme.accent; visible: tabPerms.checked } }
            }
        }

        StackLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; currentIndex: tabBar.currentIndex
            
            ColumnLayout {
                spacing: 12
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 160; color: theme.surfaceSecondary; radius: 12; border.color: theme.border
                    GridLayout {
                        anchors.fill: parent; anchors.margins: 15; columns: 2; rowSpacing: 8; columnSpacing: 20
                        Label { text: "Location"; color: theme.textMuted; font.pixelSize: 11; font.bold: true }
                        Text { text: filePath; color: theme.textPrimary; elide: Text.ElideMiddle; Layout.fillWidth: true; font.pixelSize: 12 }
                        Label { text: "Size"; color: theme.textMuted; font.pixelSize: 11; font.bold: true }
                        Text { text: humanReadableSize(fileSizeBytes); color: theme.textPrimary; font.pixelSize: 12 }
                        Label { text: "Modified"; color: theme.textMuted; font.pixelSize: 11; font.bold: true }
                        Text { text: formatDate(fileDateRaw); color: theme.textPrimary; font.pixelSize: 12 }
                        Label { text: "MIME Type"; color: theme.textMuted; font.pixelSize: 11; font.bold: true }
                        Text { text: mimeType; color: theme.textPrimary; font.pixelSize: 12 }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: !isDir
                    Text {
                        text: "Default Application"
                        color: theme.accent
                        font.pixelSize: 11; font.bold: true; font.letterSpacing: 0.8
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Change..."
                        flat: true
                        font.pixelSize: 10
                        onClicked: if (appWindow) appWindow.showOpenWith(filePath, activeController)
                        background: null
                        contentItem: Text { text: parent.text; color: theme.accent; font.bold: true }
                    }
                }

                Rectangle {
                    visible: !isDir
                    Layout.fillWidth: true; Layout.fillHeight: true; color: theme.surfaceSecondary; radius: 12; border.color: theme.border
                    clip: true
                    ListView {
                        anchors.fill: parent; anchors.margins: 5
                        model: associatedApps
                        delegate: ItemDelegate {
                            width: parent.width
                            height: 40
                            contentItem: RowLayout {
                                spacing: 10
                                Icon { name: modelData.icon || "application-x-executable"; iconSize: 18; color: theme.textPrimary }
                                Text { text: modelData.name; color: theme.textPrimary; font.pixelSize: 12; Layout.fillWidth: true }
                                RadioButton {
                                    checked: modelData.isDefault || false
                                    onClicked: {
                                        if (activeController) activeController.setDefaultApp(mimeType, modelData.id)
                                        // Refresh the list to update defaults
                                        associatedApps = activeController.getAssociatedApps(filePath)
                                    }
                                }
                            }
                            background: Rectangle { radius: 8; color: parent.hovered ? theme.hover : "transparent" }
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }

            ColumnLayout {
                spacing: 15
                GridLayout {
                    columns: 4; columnSpacing: 20; rowSpacing: 10
                    Item { Layout.preferredWidth: 60 }
                    Text { text: "R"; color: theme.accent; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                    Text { text: "W"; color: theme.accent; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                    Text { text: "X"; color: theme.accent; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                    
                    Text { text: "Owner"; color: theme.textPrimary; font.pixelSize: 13 }
                    CheckBox { checked: permModel.owner ? permModel.owner.r : false; onCheckedChanged: if(permModel.owner) permModel.owner.r = checked }
                    CheckBox { checked: permModel.owner ? permModel.owner.w : false; onCheckedChanged: if(permModel.owner) permModel.owner.w = checked }
                    CheckBox { checked: permModel.owner ? permModel.owner.x : false; onCheckedChanged: if(permModel.owner) permModel.owner.x = checked }
                    
                    Text { text: "Group"; color: theme.textPrimary; font.pixelSize: 13 }
                    CheckBox { checked: permModel.group ? permModel.group.r : false; onCheckedChanged: if(permModel.group) permModel.group.r = checked }
                    CheckBox { checked: permModel.group ? permModel.group.w : false; onCheckedChanged: if(permModel.group) permModel.group.w = checked }
                    CheckBox { checked: permModel.group ? permModel.group.x : false; onCheckedChanged: if(permModel.group) permModel.group.x = checked }
                    
                    Text { text: "Others"; color: theme.textPrimary; font.pixelSize: 13 }
                    CheckBox { checked: permModel.others ? permModel.others.r : false; onCheckedChanged: if(permModel.others) permModel.others.r = checked }
                    CheckBox { checked: permModel.others ? permModel.others.w : false; onCheckedChanged: if(permModel.others) permModel.others.w = checked }
                    CheckBox { checked: permModel.others ? permModel.others.x : false; onCheckedChanged: if(permModel.others) permModel.others.x = checked }
                }
                Text { text: "Mode: " + permsToNumeric(permModel); color: theme.textSecondary; font.family: "Monospace"; font.pixelSize: 12 }
                Item { Layout.fillHeight: true }
            }
        }

        RowLayout {
            Layout.fillWidth: true; spacing: 12
            Button {
                text: "APPLY CHANGES"
                visible: tabBar.currentIndex === 1
                Layout.fillWidth: true; Layout.preferredHeight: 40
                onClicked: changePermissionsRequested(filePath, permsToNumeric(permModel))
                background: Rectangle { radius: 8; border.color: theme.accent; color: "transparent" }
                contentItem: Text { text: parent.text; color: theme.accent; font.bold: true; font.pixelSize: 11; font.letterSpacing: 1; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
            Button {
                text: "CLOSE"
                Layout.fillWidth: true; Layout.preferredHeight: 40
                onClicked: root.close()
                background: Rectangle { radius: 8; color: theme.accent }
                contentItem: Text { text: parent.text; color: theme.isDark ? "#1C1C1C" : "#FFFFFF"; font.bold: true; font.pixelSize: 11; font.letterSpacing: 1; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
        }
    }
}
