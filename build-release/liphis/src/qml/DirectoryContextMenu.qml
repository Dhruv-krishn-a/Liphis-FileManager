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
        return ""
    }

    MenuItem {
        text: "New Folder"
        onTriggered: if (controller) controller.createFolder("New Folder")
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("folder-plus")
        icon.color: theme.accent
    }

    MenuItem {
        text: "New Empty File"
        onTriggered: if (controller) controller.createFile("untitled.txt")
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("file-plus")
        icon.color: theme.accent
    }

    MenuSeparator { topPadding: 4; bottomPadding: 4 }

    MenuItem {
        text: "Show Hidden"
        checkable: true
        checked: controller ? controller.showHiddenFiles : false
        onTriggered: if (controller) controller.showHiddenFiles = checked
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("eye")
    }

    MenuItem {
        text: "Paste"
        enabled: controller ? controller.hasClipboard : false
        onTriggered: if (controller) controller.pasteItem()
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("clipboard")
    }

    MenuSeparator { topPadding: 4; bottomPadding: 4 }

    MenuItem {
        text: "Go to Parent"
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

    MenuSeparator { topPadding: 4; bottomPadding: 4 }

    MenuItem {
        text: "Open Terminal Here"
        onTriggered: if (controller) controller.openInTerminal(root.targetPath)
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("terminal")
    }

    MenuItem {
        text: "Properties"
        onTriggered: if (controller) appWindow.showPropertiesForPath(root.targetPath, controller)
        Component.onCompleted: root.compact(this)
        icon.source: root.iconSource("info")
    }
}
