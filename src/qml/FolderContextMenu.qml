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
    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }
    
    signal openInNewTab(string path)
    signal openInNewWindow(string path)
    signal openInSplitView(string path)

    Material.theme: darkMode ? Material.Dark : Material.Light
    Material.background: theme.surfaceRaised
    Material.foreground: theme.textPrimary
    font.pixelSize: theme.fontBody - 1
    implicitWidth: 198
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
    }

    function iconSource(name) {
        if (name === "tab-new") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/square-plus.svg"
        if (name === "window-new") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/app-window.svg"
        if (name === "view-split-left-right") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-board-split.svg"
        if (name === "edit-cut") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/scissors.svg"
        if (name === "edit-copy") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/copy.svg"
        if (name === "edit-rename") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/pencil.svg"
        if (name === "user-trash") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/trash.svg"
        if (name === "view-list-details" || name === "document-properties") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/list-details.svg"
        return ""
    }

    MenuItem { 
        text: "Open in New Tab"
        Component.onCompleted: root.compact(this)
        onTriggered: root.openInNewTab(root.targetPath)
        icon.source: root.iconSource("tab-new")
    }
    MenuItem { 
        text: "Open in New Window"
        Component.onCompleted: root.compact(this)
        onTriggered: root.openInNewWindow(root.targetPath)
        icon.source: root.iconSource("window-new")
    }
    MenuItem { 
        text: "Open in Split View"
        Component.onCompleted: root.compact(this)
        onTriggered: root.openInSplitView(root.targetPath)
        icon.source: root.iconSource("view-split-left-right")
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
        text: "Open Terminal Here"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.openInTerminal(root.targetPath)
        icon.source: root.iconSource("view-list-details")
    }
    
    MenuSeparator { topPadding: 2; bottomPadding: 2 }
    
    MenuItem {
        text: "Analyse Disk Usage"
        Component.onCompleted: root.compact(this)
        onTriggered: if (controller) controller.analyseFolder(root.targetPath)
        icon.source: root.iconSource("document-properties")
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
