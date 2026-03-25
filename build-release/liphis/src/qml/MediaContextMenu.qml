import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Menu {
    id: root
    property string targetPath: ""
    property string targetName: ""
    property string mimeType: ""
    property var controller: null
    property var propsDialog: null
    property var toastManager: null

    Material.theme: Material.Dark
    Material.background: "#1a1c2e"

    MenuItem { 
        text: "Open"
        onTriggered: if (controller) controller.openPath(root.targetPath)
        icon.name: "media-playback-start"
    }
    
    MenuItem { 
        text: "Set as Wallpaper"
        visible: root.mimeType.startsWith("image/")
        onTriggered: if (controller) controller.setWallpaper(root.targetPath)
        icon.name: "background"
    }
    
    MenuSeparator { }

    MenuItem { 
        text: "Cut"
        onTriggered: if (controller) controller.cutItem(root.targetPath)
        icon.name: "edit-cut"
    }
    MenuItem { 
        text: "Copy"
        onTriggered: if (controller) controller.copyItem(root.targetPath)
        icon.name: "edit-copy"
    }
    
    MenuSeparator { }

    // Tag Color Selector (as seen in screenshot)
    MenuItem {
        height: 48
        background: Item {}
        contentItem: RowLayout {
            spacing: 8
            Repeater {
                model: ["#ef4444", "#f59e0b", "#10b981", "#3b82f6", "#8b5cf6"]
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
            Icon { name: "tag"; size: 16; color: "gray" }
        }
    }

    MenuSeparator { }

    MenuItem { 
        text: "Rename..."
        onTriggered: if (controller) controller.startRename(root.targetPath)
        icon.name: "edit-rename"
    }
    MenuItem { 
        text: "Move to Trash"
        onTriggered: if (controller) controller.trashItems([root.targetPath])
        icon.name: "user-trash"
    }

    MenuItem { 
        text: "Properties"
        onTriggered: {
            if (controller && propsDialog) propsDialog.show(controller.metadataForPath(root.targetPath))
        }
        icon.name: "document-properties"
    }
}
