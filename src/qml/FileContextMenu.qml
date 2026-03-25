import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Menu {
    id: root
    property string targetPath: ""
    property string targetName: ""
    property var controller: null
    property var propsDialog: null
    property var toastManager: null
    property bool darkMode: false

    Material.theme: darkMode ? Material.Dark : Material.Light
    Material.background: darkMode ? "#2B2723" : "#ffffff"

    MenuItem { 
        text: "Open"
        onTriggered: if (controller) controller.openPath(root.targetPath)
        icon.name: "document-open"
    }
    MenuItem { 
        text: "Open in Code"
        onTriggered: if (controller) controller.openInCode(root.targetPath)
        icon.name: "vscode"
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
    MenuItem { 
        text: "Copy Location"
        onTriggered: if (controller) controller.copyToClipboard(root.targetPath)
        icon.name: "edit-copy"
    }
    
    MenuSeparator { }

    MenuItem { 
        text: "Duplicate"
        onTriggered: if (controller) controller.duplicateItem(root.targetPath)
        icon.name: "edit-copy"
    }
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

    MenuSeparator { }
    
    MenuItem {
        text: "Compute Checksum"
        onTriggered: {
            if (!controller) return;
            var sum = controller.computeChecksum(root.targetPath);
            if (toastManager) toastManager.show("SHA256: " + sum, 5000);
        }
        icon.name: "dialog-password"
    }

    MenuItem { 
        text: "Properties"
        onTriggered: {
            if (controller && propsDialog) propsDialog.show(controller.metadataForPath(root.targetPath))
        }
        icon.name: "document-properties"
    }
}
