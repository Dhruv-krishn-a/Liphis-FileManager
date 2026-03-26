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
        if (name === "document-open") return "assets/icons/outline/folder-open.svg"
        if (name === "vscode") return "assets/icons/outline/brand-vscode.svg"
        if (name === "edit-cut") return "assets/icons/outline/scissors.svg"
        if (name === "edit-copy") return "assets/icons/outline/copy.svg"
        if (name === "edit-rename") return "assets/icons/outline/pencil.svg"
        if (name === "user-trash") return "assets/icons/outline/trash.svg"
        if (name === "dialog-password") return "assets/icons/outline/password.svg"
        if (name === "document-properties") return "assets/icons/outline/list-details.svg"
        return ""
    }

    MenuItem { 
        text: "Open"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openPath(root.targetPath)
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
    MenuItem { 
        text: "Copy Location"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.copyToClipboard(root.targetPath)
        icon.source: root.iconSource("edit-copy")
    }
    
    MenuSeparator { topPadding: 2; bottomPadding: 2 }

    MenuItem { 
        text: "Duplicate"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.duplicateItem(root.targetPath)
        icon.source: root.iconSource("edit-copy")
    }
    MenuItem { 
        text: "Rename..."
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.startRename(root.targetPath)
        icon.source: root.iconSource("edit-rename")
    }
    MenuItem { 
        text: "Move to Trash"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.trashItems([root.targetPath])
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
