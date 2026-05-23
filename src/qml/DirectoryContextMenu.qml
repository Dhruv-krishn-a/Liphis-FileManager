import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import liphis

Menu {
    id: root
    property string targetPath: ""
    property var controller: null
    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }

    Material.theme: darkMode ? Material.Dark : Material.Light
    Material.background: theme.surfaceRaised
    Material.foreground: theme.textPrimary
    font.pixelSize: theme.fontBody - 1
    implicitWidth: 210
    topPadding: 6
    bottomPadding: 6

    background: Rectangle {
        radius: theme.rSm
        color: theme.surfaceRaised
        border.color: theme.border
        border.width: 1
    }

    function compact(item) {
        item.height = 34;
        item.leftPadding = 12;
        item.rightPadding = 12;
    }

    function iconSource(name) {
        if (name === "folder-plus") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-plus.svg"
        if (name === "file-plus") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/square-plus.svg"
        if (name === "eye") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/eye.svg"
        if (name === "clipboard") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/clipboard.svg"
        if (name === "arrow-up") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/arrow-up.svg"
        if (name === "refresh") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/refresh.svg"
        if (name === "terminal") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/terminal.svg"
        if (name === "info") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/info-circle.svg"
        if (name === "user-trash") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/trash.svg"
        if (name === "zoom-in") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/zoom-in.svg"
        if (name === "zoom-out") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/zoom-out.svg"
        if (name === "zoom-reset") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/zoom-reset.svg"
        if (name === "file") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/file.svg"
        if (name === "doc-text") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/file-description.svg"
        if (name === "doc-spreadsheet") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/file-spreadsheet.svg"
        if (name === "eye") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/eye.svg"
        return ""
    }

    MenuItem {
        text: "Create Folder..."
        onTriggered: if (centerWorkspace.activeView) centerWorkspace.activeView.openCreateFolderDialog()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("folder-plus")
        icon.color: theme.accent
    }

    Menu {
        title: "Create Document"
        icon.source: root.iconSource("file-plus")
        icon.color: theme.accent
        Action { 
            text: "Empty Text File"; icon.source: root.iconSource("doc-text");
            onTriggered: if (controller) controller.createFile("untitled.txt") 
        }
        Action { 
            text: "Rich Text Document"; icon.source: root.iconSource("file");
            onTriggered: if (controller) controller.createFile("untitled.odt") 
        }
        Action { 
            text: "Spreadsheet"; icon.source: root.iconSource("doc-spreadsheet");
            onTriggered: if (controller) controller.createFile("untitled.ods") 
        }
    }

    MenuSeparator { topPadding: 4; bottomPadding: 4 }

    MenuItem {
        text: "Paste"
        enabled: controller ? controller.hasClipboard : false
        onTriggered: if (controller) appWindow.requestPaste(controller)
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("clipboard")
    }

    MenuItem {
        text: "Show Hidden Files"
        checkable: true
        checked: controller ? controller.showHiddenFiles : false
        onTriggered: if (controller) controller.showHiddenFiles = checked
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("eye")
    }

    MenuItem {
        text: "Go to Parent"
        enabled: controller ? !controller.currentPath.startsWith("trash:") : false
        onTriggered: if (controller) controller.goUp()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("arrow-up")
    }

    MenuItem {
        text: "Refresh"
        onTriggered: if (controller) controller.refresh()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("refresh")
    }

    MenuItem {
        text: "Open Terminal Here"
        onTriggered: if (controller) controller.openInTerminal(root.targetPath)
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("terminal")
    }

    MenuSeparator { topPadding: 4; bottomPadding: 4 }

    Menu {
        title: "Arrange Items"
        icon.source: root.iconSource("refresh")
        Action { text: "by Name"; checkable: true; checked: viewSettings.sortField === "name"; onTriggered: controller.fileModel.setSortBy("name") }
        Action { text: "by Size"; checkable: true; checked: viewSettings.sortField === "size"; onTriggered: controller.fileModel.setSortBy("size") }
        Action { text: "by Type"; checkable: true; checked: viewSettings.sortField === "type"; onTriggered: controller.fileModel.setSortBy("type") }
        Action { text: "by Modification Date"; checkable: true; checked: viewSettings.sortField === "date"; onTriggered: controller.fileModel.setSortBy("date") }
        MenuSeparator {}
        Action { text: "Ascending"; checkable: true; checked: viewSettings.sortAscending; onTriggered: controller.fileModel.setSortBy(viewSettings.sortField, true) }
        Action { text: "Descending"; checkable: true; checked: !viewSettings.sortAscending; onTriggered: controller.fileModel.setSortBy(viewSettings.sortField, false) }
    }

    MenuSeparator { topPadding: 4; bottomPadding: 4 }

    MenuItem {
        text: "Zoom In"
        onTriggered: if (centerWorkspace.activeView) centerWorkspace.activeView.zoomIn()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("zoom-in")
    }

    MenuItem {
        text: "Zoom Out"
        onTriggered: if (centerWorkspace.activeView) centerWorkspace.activeView.zoomOut()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("zoom-out")
    }

    MenuItem {
        text: "Normal Size"
        onTriggered: if (centerWorkspace.activeView) centerWorkspace.activeView.resetZoom()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("zoom-reset")
    }

    MenuSeparator { 
        visible: controller ? controller.currentPath.startsWith("trash:") : false
        topPadding: 4; bottomPadding: 4 
    }

    MenuItem {
        text: "Empty Trash"
        visible: controller ? controller.currentPath.startsWith("trash:") : false
        onTriggered: if (controller) appWindow.requestEmptyTrash(controller)
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("user-trash")
    }
}
