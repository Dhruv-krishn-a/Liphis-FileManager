import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import liphis

Menu {
    id: root
    property string targetPath: ""
    property string targetName: ""
    property string mimeType: ""
    property var controller: null
    property var propsDialog: null
    property var toastManager: null
    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }

    Material.theme: darkMode ? Material.Dark : Material.Light
    Material.background: theme.surfaceRaised
    Material.foreground: theme.textPrimary
    font.pixelSize: theme.fontBody - 1
    implicitWidth: 200
    topPadding: 4
    bottomPadding: 4

    background: Rectangle {
        radius: theme.rSm
        color: theme.surfaceRaised
        border.color: theme.border
        border.width: 1
    }

    function compact(item) {
        item.height = 32;
        item.leftPadding = 10;
        item.rightPadding = 10;
        item.icon.width = 16;
        item.icon.height = 16;
    }

    function iconSource(name) {
        if (name === "media-playback-start") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/player-play.svg"
        if (name === "background") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/background.svg"
        if (name === "edit-cut") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/scissors.svg"
        if (name === "edit-copy") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/copy.svg"
        if (name === "edit-rename") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/pencil.svg"
        if (name === "user-trash") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/trash.svg"
        if (name === "document-properties") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/list-details.svg"
        return ""
    }

    MenuItem { 
        text: "Open"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openPath(root.targetPath)
        icon.source: root.iconSource("media-playback-start")
    }
    MenuItem {
        text: "Open With..."
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openWith(root.targetPath)
        icon.source: root.iconSource("document-open")
    }
    
    MenuItem { 
        text: "Set as Wallpaper"
        Component.onCompleted: root.compact(this)
        visible: root.mimeType.startsWith("image/")
        onTriggered: if (controller) controller.setWallpaper(root.targetPath)
        icon.source: root.iconSource("background")
    }
    
    MenuSeparator { topPadding: 2; bottomPadding: 2 }

    MenuItem { 
        text: "Cut"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.cutItem(root.targetPath)
        icon.source: root.iconSource("edit-cut")
    }
    MenuItem { 
        text: "Copy"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.copyItem(root.targetPath)
        icon.source: root.iconSource("edit-copy")
    }
    
    MenuSeparator { topPadding: 2; bottomPadding: 2 }

    // Tag Color Selector (as seen in screenshot)
    MenuItem {
        height: 48
        background: Item {}
        contentItem: RowLayout {
            spacing: 8
            Repeater {
                model: ["#C9645A", "#C2945D", "#77A572", "#6E90B8", "#9E86B8"]
                delegate: Rectangle {
                    width: 24; height: 24; radius: 4
                    color: modelData
                    border.color: "white"
                    border.width: mouseArea.containsMouse ? 2 : 0
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (toastManager) toastManager.show("Tag applied to " + root.targetName);
                            root.dismiss();
                        }
                    }
                }
            }
            Item { Layout.fillWidth: true }
            Icon { name: "tag"; iconSize: 16; color: theme.textMuted }
        }
    }

    MenuSeparator { topPadding: 2; bottomPadding: 2 }

    MenuItem { 
        text: "Rename..."
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.startRename(root.targetPath)
        icon.source: root.iconSource("edit-rename")
    }
    MenuItem { 
        text: "Move to Trash"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) appWindow.requestTrash(controller, [root.targetPath])
        icon.source: root.iconSource("user-trash")
    }

    MenuItem { 
        text: "Properties"
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (controller && propsDialog) propsDialog.show(controller.metadataForPath(root.targetPath))
        }
        icon.source: root.iconSource("document-properties")
    }
}
