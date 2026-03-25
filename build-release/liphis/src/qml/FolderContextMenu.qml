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
    
    signal openInNewTab(string path)
    signal openInNewWindow(string path)
    signal openInSplitView(string path)

    Material.theme: Material.Dark
    Material.background: "#1a1c2e"

    MenuItem { 
        text: "Open in New Tab"
        onTriggered: root.openInNewTab(root.targetPath)
        icon.name: "tab-new"
    }
    MenuItem { 
        text: "Open in New Window"
        onTriggered: root.openInNewWindow(root.targetPath)
        icon.name: "window-new"
    }
    MenuItem { 
        text: "Open in Split View"
        onTriggered: root.openInSplitView(root.targetPath)
        icon.name: "view-split-left-right"
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
        text: "Open Terminal Here"
        onTriggered: if (controller) controller.openInTerminal(root.targetPath)
        icon.name: "utilities-terminal"
    }
    
    MenuSeparator { }
    
    MenuItem {
        text: "Analyse Disk Usage"
        onTriggered: if (controller) controller.analyseFolder(root.targetPath)
        icon.name: "dialog-information"
    }

    MenuItem { 
        text: "Properties"
        onTriggered: {
            if (controller && propsDialog) propsDialog.show(controller.metadataForPath(root.targetPath))
        }
        icon.name: "document-properties"
    }
}
