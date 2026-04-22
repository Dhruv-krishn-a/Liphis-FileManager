import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import liphis

Menu {
    id: root
    property string targetPath: ""
    property string targetName: ""
    property var controller: null
    property var propsDialog: null
    property var toastManager: null
    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }

    Material.theme: darkMode ? Material.Dark : Material.Light
    Material.background: theme.surfaceRaised
    Material.foreground: theme.textPrimary
    font.pixelSize: theme.fontBody - 1
    implicitWidth: 194
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
        if (name === "document-open") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-open.svg"
        if (name === "vscode") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/brand-vscode.svg"
        if (name === "edit-cut") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/scissors.svg"
        if (name === "edit-copy") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/copy.svg"
        if (name === "edit-rename") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/pencil.svg"
        if (name === "user-trash") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/trash.svg"
        if (name === "dialog-password") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/password.svg"
        if (name === "document-properties") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/list-details.svg"
        return ""
    }

    MenuItem {
        text: "Open"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openPath(root.targetPath)
        icon.source: root.iconSource("document-open")
    }
    MenuItem {
        text: "Open With..."
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openWith(root.targetPath)
        icon.source: root.iconSource("document-open")
    }
    MenuItem {
        text: "Open in Code"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openInCode(root.targetPath)
        icon.source: root.iconSource("vscode")
    }
    
    MenuSeparator { topPadding: 2; bottomPadding: 2 }

    MenuItem { 
        text: controller && controller.selectedPaths.length > 1 ? qsTr("Cut %1 Items").arg(controller.selectedPaths.length) : "Cut"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.cutItem("") // passing empty uses selection
        icon.source: root.iconSource("edit-cut")
    }
    MenuItem { 
        text: controller && controller.selectedPaths.length > 1 ? qsTr("Copy %1 Items").arg(controller.selectedPaths.length) : "Copy"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.copyItem("") // passing empty uses selection
        icon.source: root.iconSource("edit-copy")
    }
    MenuItem { 
        text: "Copy Location"
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (!controller) return;
            if (controller.selectedPaths.length > 1) controller.copyToClipboard(controller.selectedPaths.join("\n"));
            else controller.copyToClipboard(root.targetPath);
        }
        icon.source: root.iconSource("edit-copy")
    }
    
    MenuSeparator { topPadding: 2; bottomPadding: 2 }

    MenuItem { 
        text: "Duplicate"
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (!controller) return;
            if (controller.selectedPaths.length > 1) {
                for (var i=0; i < controller.selectedPaths.length; i++) controller.duplicateItem(controller.selectedPaths[i]);
            } else controller.duplicateItem(root.targetPath);
        }
        icon.source: root.iconSource("edit-copy")
    }
    MenuItem { 
        text: "Rename..."
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) {
            if (controller.selectedPaths.length > 1) {
                // Future: show bulk rename
                if (appWindow) appWindow.showBulkRename(controller.selectedPaths);
            } else {
                controller.startRename(root.targetPath);
            }
        }
        icon.source: root.iconSource("edit-rename")
    }
    MenuItem { 
        text: "Move to Trash"
        visible: controller ? !controller.currentPath.startsWith("trash:") : true
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (!controller) return;
            if (controller.selectedPaths.length > 1) controller.trashItems(controller.selectedPaths);
            else controller.trashItems([root.targetPath]);
        }
        icon.source: root.iconSource("user-trash")
    }

    MenuItem {
        text: "Restore"
        visible: controller ? controller.currentPath.startsWith("trash:") : false
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (!controller) return;
            if (controller.selectedPaths.length > 0) controller.restoreFromTrash(controller.selectedPaths);
            else controller.restoreFromTrash([root.targetPath]);
        }
        icon.source: root.iconSource("document-open")
    }

    MenuItem {
        text: "Delete Permanently"
        visible: controller ? controller.currentPath.startsWith("trash:") : false
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (!controller) return;
            if (controller.selectedPaths.length > 1) controller.deleteItems(controller.selectedPaths);
            else controller.deleteItems([root.targetPath]);
        }
        icon.source: root.iconSource("user-trash")
    }

    MenuSeparator { topPadding: 2; bottomPadding: 2 }
    
    MenuItem {
        text: "Compute Checksum"
        Component.onCompleted: root.compact(this)
        onTriggered: {
            if (!controller) return;
            var sum = controller.computeChecksum(root.targetPath);
            if (toastManager) toastManager.show("SHA256: " + sum, 5000);
        }
        icon.source: root.iconSource("dialog-password")
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
