import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Liphis.Core 1.0

Item {
    id: root
    
    property string initialPath: ""
    property alias controller: controller
    property var theme: null 

    signal requestedActive()

    Rectangle {
        anchors.fill: parent
        color: theme ? theme.bg : "#12141c"
    }

    AppController {
        id: controller
        Component.onCompleted: {
            if (typeof globalThumbnailManager !== "undefined") setThumbnailManager(globalThumbnailManager);
            if (typeof globalPlacesModel !== "undefined") setPlacesModel(globalPlacesModel);
            var launchPath = initialPath && initialPath.length > 0 ? initialPath : homePath;
            if (launchPath && launchPath.length > 0) openPath(launchPath);
            root.requestedActive()
        }
        onRenameRequested: (path) => renamingPath = path
        onCurrentPathChanged: {
            // Memory Optimization: Force garbage collection on major navigation
            gc();
        }
    }
    
    Component.onCompleted: {
        if (typeof updateActiveController === "function") updateActiveController()
    }

    readonly property string homePath: controller ? controller.homePath : ""

    readonly property color colTextPrimary: theme ? theme.textPrimary : "#F3F1ED"
    readonly property color colTextSecondary: theme ? theme.textSecondary : "#B5B1AA"
    property string renamingPath: ""

    Dialog {
        id: createFolderDialog
        title: "Create Folder"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 340
        anchors.centerIn: parent

        property alias folderName: folderNameField.text

        contentItem: ColumnLayout {
            spacing: 10
            Text { text: "Folder name"; color: root.colTextSecondary; font.pixelSize: 12 }
            TextField {
                id: folderNameField
                Layout.fillWidth: true
                placeholderText: "New Folder"
                selectByMouse: true
            }
        }

        onOpened: {
            folderNameField.text = ""
            folderNameField.forceActiveFocus()
        }
        onAccepted: {
            if (folderNameField.text.trim().length > 0) controller.createFolder(folderNameField.text.trim())
        }
    }

    // Context Menu for Background
    Menu {
        id: bgMenu
        Material.theme: (theme && theme.isDark) ? Material.Dark : Material.Light
        Material.background: theme ? theme.surfaceElevated : "#FFFFFF"
        MenuItem { text: "New Folder"; onTriggered: createFolderDialog.open() }
        MenuItem { text: "Show Hidden"; checkable: true; checked: controller.showHiddenFiles; onTriggered: controller.showHiddenFiles = checked }
        MenuItem { text: "Paste"; enabled: controller.hasClipboard; onTriggered: controller.pasteItem() }
        MenuItem { text: "Refresh"; onTriggered: controller.refresh() }
    }

    // --- DELEGATES ---

    Component {
        id: listDelegate
        Item {
            id: listRoot
            width: lv.width; height: 44 // A bit more breathing room
            readonly property bool isActuallySelected: controller.selectedPaths.indexOf(model.path) !== -1

            Rectangle {
                anchors.fill: parent; anchors.margins: 2; radius: theme ? theme.radiusSmall : 6
                color: listRoot.isActuallySelected ? (theme ? theme.selection : "#33ffffff") : (listMA.containsMouse ? (theme ? theme.hover : "#11ffffff") : "transparent")
                visible: listRoot.isActuallySelected || listMA.containsMouse
            }

            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 16
                FileIcon { Layout.preferredWidth: 22; Layout.preferredHeight: 22; isDir: model.isDir; iconName: model.iconName }
                Text { text: model.name; Layout.fillWidth: true; elide: Text.ElideRight; color: root.colTextPrimary; font.pixelSize: 13; font.weight: listRoot.isActuallySelected ? Font.Medium : Font.Normal }
                Text { text: model.formattedSize; Layout.preferredWidth: 80; horizontalAlignment: Text.AlignRight; color: root.colTextSecondary; font.pixelSize: 12 }
                Text { text: model.formattedDate; Layout.preferredWidth: 140; horizontalAlignment: Text.AlignRight; color: root.colTextSecondary; font.pixelSize: 12 }
            }

            MouseArea {
                id: listMA; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: (mouse) => {
                    if (mouse.modifiers & Qt.ControlModifier) controller.toggleSelection(model.path)
                    else controller.selectPath(model.path)
                    if (mouse.button === Qt.RightButton) {
                        if (typeof showItemMenu === "function") showItemMenu(model, controller)
                    }
                }
                onDoubleClicked: controller.openPath(model.path)
            }
        }
    }

    Component {
        id: gridDelegate
        Item {
            id: gridRoot
            width: gv.cellWidth; height: gv.cellHeight
            readonly property bool isActuallySelected: controller.selectedPaths.indexOf(model.path) !== -1

            Rectangle {
                anchors.fill: parent; anchors.margins: 4; radius: theme ? theme.radius : 8
                color: gridRoot.isActuallySelected ? (theme ? theme.selection : "#33ffffff") : (gridMA.containsMouse ? (theme ? theme.hover : "#11ffffff") : "transparent")
                visible: gridRoot.isActuallySelected || gridMA.containsMouse
            }

            ColumnLayout {
                anchors.fill: parent; anchors.margins: 12; spacing: 8
                Item {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    FileIcon { anchors.centerIn: parent; width: 56; height: 56; isDir: model.isDir; iconName: model.iconName; visible: !gridThumb.visible }
                    Image { 
                        id: gridThumb; anchors.fill: parent; fillMode: Image.PreserveAspectFit; source: model.thumbnail ? model.thumbnail : ""
                        visible: status === Image.Ready; asynchronous: false; cache: true // Sync for memory predictability
                    }
                }
                Text { 
                    text: model.name; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight
                    color: root.colTextPrimary; font.pixelSize: 12; font.weight: gridRoot.isActuallySelected ? Font.Medium : Font.Normal
                }
            }

            MouseArea {
                id: gridMA; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: (mouse) => {
                    if (mouse.modifiers & Qt.ControlModifier) controller.toggleSelection(model.path)
                    else controller.selectPath(model.path)
                    if (mouse.button === Qt.RightButton) {
                        if (typeof showItemMenu === "function") showItemMenu(model, controller)
                    }
                }
                onDoubleClicked: controller.openPath(model.path)
            }
        }
    }

    // --- MAIN VIEW ---

    StackLayout {
        anchors.fill: parent
        currentIndex: controller.viewMode === "grid" ? 1 : 0

        ListView {
            id: lv; model: controller.fileModel; clip: true; delegate: listDelegate
            cacheBuffer: 0 // Absolute minimum memory
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }

        GridView {
            id: gv; model: controller.fileModel; clip: true; delegate: gridDelegate
            cellWidth: 100; cellHeight: 100; cacheBuffer: 0
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }
    }

    MouseArea {
        anchors.fill: parent; z: -1; acceptedButtons: Qt.RightButton | Qt.BackButton | Qt.ForwardButton | Qt.LeftButton
        onClicked: (mouse) => {
            if (mouse.button === Qt.RightButton) bgMenu.popup()
            else if (mouse.button === Qt.BackButton) controller.goBack()
            else if (mouse.button === Qt.ForwardButton) controller.goForward()
            else if (mouse.button === Qt.LeftButton) controller.clearSelection()
        }
    }
}
