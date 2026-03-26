import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import QtCore
import Liphis.Core 1.0

ApplicationWindow {
    id: appWindow
    objectName: "mainWindow"
    width: 1200
    height: 800
    visible: true
    title: activeController ? activeController.title + " - Liphis" : "Liphis"

    AppTheme { id: appTheme }
    property alias theme: appTheme

    property var activeController: null
    property bool detailsVisible: true
    property bool terminalVisible: false
    property bool inspectorExpanded: true
    property string homePath: resolveHomePath()

    Settings {
        id: uiSettings
        category: "ui"
        property bool darkMode: false
        property bool detailsPanelVisible: true
        property bool terminalPanelVisible: false
        property bool inspectorExpanded: true
        property int windowWidth: 1200
        property int windowHeight: 800
    }

    Component.onCompleted: {
        appWindow.width = Math.max(960, uiSettings.windowWidth)
        appWindow.height = Math.max(640, uiSettings.windowHeight)
        appWindow.theme.isDark = uiSettings.darkMode
        appWindow.detailsVisible = uiSettings.detailsPanelVisible
        appWindow.terminalVisible = uiSettings.terminalPanelVisible
        appWindow.inspectorExpanded = uiSettings.inspectorExpanded
    }

    onWidthChanged: uiSettings.windowWidth = width
    onHeightChanged: uiSettings.windowHeight = height
    onDetailsVisibleChanged: uiSettings.detailsPanelVisible = detailsVisible
    onTerminalVisibleChanged: uiSettings.terminalPanelVisible = terminalVisible
    onInspectorExpandedChanged: uiSettings.inspectorExpanded = inspectorExpanded
    Connections {
        target: appWindow.theme
        function onIsDarkChanged() { uiSettings.darkMode = appWindow.theme.isDark }
    }

    background: Rectangle { color: theme.bg }

    function normalizePath(path) {
        if (!path) return ""
        var p = String(path)
        if (p.indexOf("file:///") === 0) {
            p = decodeURIComponent(p.substring("file://".length))
        } else if (p.indexOf("file://") === 0) {
            p = decodeURIComponent(p.substring("file://".length))
        }
        return p
    }

    function resolveHomePath() {
        if (activeController && activeController.homePath && activeController.homePath.length > 0) {
            return normalizePath(activeController.homePath)
        }
        return normalizePath(StandardPaths.writableLocation(StandardPaths.HomeLocation))
    }

    function addTab(path) {
        centerWorkspace.addTab(normalizePath(path))
    }

    function isMediaFile(name) {
        var dot = name.lastIndexOf(".")
        if (dot < 0) return false
        var ext = name.substring(dot + 1).toLowerCase()
        return ["png","jpg","jpeg","webp","gif","bmp","mp4","mkv","avi","mov","mp3","wav","flac","ogg"].indexOf(ext) !== -1
    }

    function showItemMenu(model, controllerObj) {
        if (!model || !controllerObj) return
        if (model.isDir) {
            folderMenu.controller = controllerObj
            folderMenu.targetPath = model.path
            folderMenu.targetName = model.name
            folderMenu.popup()
            return
        }
        if (isMediaFile(model.name)) {
            mediaMenu.controller = controllerObj
            mediaMenu.targetPath = model.path
            mediaMenu.targetName = model.name
            mediaMenu.popup()
            return
        }
        fileMenu.controller = controllerObj
        fileMenu.targetPath = model.path
        fileMenu.targetName = model.name
        fileMenu.popup()
    }

    onActiveControllerChanged: {
        homePath = resolveHomePath()
        if (activeController) {
            activeController.setThumbnailManager(globalThumbnailManager)
            activeController.setPlacesModel(globalPlacesModel)
            activeController.refresh()
        }
    }

    Connections {
        target: activeController
        enabled: !!activeController
        function onOperationSuccess(message) { toastManager.show(message, false) }
        function onOperationError(message) { toastManager.show(message, true) }
        function onAnalyseRequested(path) {
            analyseDialog.resetDialog()
            analysisController.analyseFolder(path)
        }
    }

    AnalysisController {
        id: analysisController
        onNodeFound: (node) => analyseDialog.addNode(node)
        onAnalysisProgress: (size, files, dirs, currentItem) => analyseDialog.setProgress(size, files, dirs, currentItem)
        onAnalysisFinished: (_result) => analyseDialog.finish()
    }

    header: HeaderBar {
        id: headerBar
        theme: appWindow.theme
        activeController: appWindow.activeController
        detailsVisible: appWindow.detailsVisible
        terminalVisible: appWindow.terminalVisible
        onDetailsToggleRequested: appWindow.detailsVisible = !appWindow.detailsVisible
        onTerminalToggleRequested: appWindow.terminalVisible = !appWindow.terminalVisible
    }

    Shortcut {
        sequences: [StandardKey.Find]
        onActivated: headerBar.focusSearchField()
    }

    Shortcut {
        sequence: "Ctrl+L"
        onActivated: headerBar.focusPathField()
    }

    Shortcut {
        sequence: "Ctrl+Shift+N"
        onActivated: if (centerWorkspace.activeView) centerWorkspace.activeView.openCreateFolderDialog()
    }

    Shortcut {
        sequence: "Ctrl+K"
        onActivated: {
            commandQuery.text = ""
            commandPalette.open()
            commandQuery.forceActiveFocus()
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            if (!activeController) return
            if (headerBar.hasActiveSearch()) headerBar.clearSearchAndRestore()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            LeftSidebar {
                id: leftSidebar
                Layout.preferredWidth: theme.sidebarWidth
                Layout.fillHeight: true
                theme: appWindow.theme
                placesModel: globalPlacesModel
                activePath: activeController ? activeController.currentPath : ""
                homePath: appWindow.homePath
                onPathActivated: (path) => {
                    if (activeController) activeController.openPath(path)
                }
                onRemoveBookmarkRequested: (path) => {
                    if (activeController) activeController.removeBookmarkByPath(path)
                }
            }

            SplitView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                orientation: Qt.Horizontal
                handle: Rectangle { implicitWidth: 1; color: theme.border }

                CenterWorkspace {
                    id: centerWorkspace
                    SplitView.fillWidth: true
                    theme: appWindow.theme
                    homePath: appWindow.homePath
                    terminalVisible: appWindow.terminalVisible
                    itemMenuHandler: appWindow.showItemMenu
                    onControllerChanged: (controller) => appWindow.activeController = controller
                }

                InspectorPanel {
                    id: inspectorPanel
                    visible: appWindow.detailsVisible
                    SplitView.preferredWidth: appWindow.inspectorExpanded
                        ? theme.inspectorWidth
                        : 252
                    SplitView.minimumWidth: 220
                    SplitView.maximumWidth: 560
                    theme: appWindow.theme
                    activeController: appWindow.activeController
                    expanded: appWindow.inspectorExpanded
                    onToggleExpandRequested: appWindow.inspectorExpanded = !appWindow.inspectorExpanded
                    onAnalyseFolderRequested: (path) => {
                        if (activeController) activeController.analyseFolder(path)
                    }
                }
            }
        }

        StatusBar {
            Layout.fillWidth: true
            theme: appWindow.theme
            activeController: appWindow.activeController
        }
    }

    PropertiesDialog {
        id: propsDialog
        darkMode: theme.isDark
        onChangePermissionsRequested: (path, newPerms) => {
            if (activeController) activeController.setPermissions(path, newPerms)
        }
    }

    AnalyseDialog {
        id: analyseDialog
        analysisController: analysisController
        toastManager: toastManager
        darkMode: theme.isDark
    }

    FileContextMenu {
        id: fileMenu
        propsDialog: propsDialog
        toastManager: toastManager
        darkMode: theme.isDark
    }

    FolderContextMenu {
        id: folderMenu
        propsDialog: propsDialog
        darkMode: theme.isDark
        onOpenInNewTab: (path) => appWindow.addTab(path)
        onOpenInNewWindow: (path) => { if (activeController) activeController.openInNewWindow(path) }
        onOpenInSplitView: (path) => centerWorkspace.openSplit(path)
    }

    MediaContextMenu {
        id: mediaMenu
        propsDialog: propsDialog
        toastManager: toastManager
        darkMode: theme.isDark
    }

    ToastManager {
        id: toastManager
        darkMode: theme.isDark
    }

    Popup {
        id: commandPalette
        modal: true
        focus: true
        width: Math.min(520, appWindow.width - 40)
        padding: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        x: Math.round((appWindow.width - width) / 2)
        y: Math.round((appWindow.height - implicitHeight) / 2)
        background: Rectangle {
            radius: theme.rMd
            color: theme.surfaceRaised
            border.color: theme.border
            border.width: 1
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                height: 46
                color: "transparent"
                border.color: theme.border
                border.width: 0
                TextField {
                    id: commandQuery
                    anchors.fill: parent
                    anchors.margins: 8
                    placeholderText: "Type a command..."
                    onAccepted: {
                        for (var i = 0; i < commandModel.count; ++i) {
                            var item = commandModel.get(i)
                            if (item.label.toLowerCase().indexOf(commandQuery.text.trim().toLowerCase()) >= 0) {
                                if (item.key === "home" && activeController) activeController.openPath(homePath)
                                else if (item.key === "inspector") detailsVisible = !detailsVisible
                                else if (item.key === "terminal") terminalVisible = !terminalVisible
                                else if (item.key === "search") headerBar.focusSearchField()
                                else if (item.key === "refresh" && activeController) activeController.refresh()
                                else if (item.key === "new_folder" && centerWorkspace.activeView) centerWorkspace.activeView.openCreateFolderDialog()
                                else if (item.key === "new_tab_home") addTab(homePath)
                                commandPalette.close()
                                break
                            }
                        }
                    }
                    background: Rectangle {
                        radius: theme.rSm
                        color: theme.surface
                        border.color: theme.border
                        border.width: 1
                    }
                }
            }
            ListView {
                Layout.fillWidth: true
                Layout.preferredHeight: 260
                clip: true
                model: ListModel {
                    id: commandModel
                    ListElement { label: "Go Home"; key: "home" }
                    ListElement { label: "Toggle Inspector"; key: "inspector" }
                    ListElement { label: "Toggle Terminal"; key: "terminal" }
                    ListElement { label: "Focus Search"; key: "search" }
                    ListElement { label: "Refresh Current Folder"; key: "refresh" }
                    ListElement { label: "New Folder"; key: "new_folder" }
                    ListElement { label: "New Tab (Home)"; key: "new_tab_home" }
                }
                delegate: Item {
                    width: parent.width
                    readonly property bool matches: commandQuery.text.trim().length === 0
                        || label.toLowerCase().indexOf(commandQuery.text.trim().toLowerCase()) >= 0
                    height: matches ? 36 : 0
                    visible: matches
                    Rectangle {
                        anchors.fill: parent
                        color: area.containsMouse ? theme.hover : "transparent"
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        text: label
                        color: theme.textPrimary
                        font.pixelSize: theme.fontBody
                    }
                    MouseArea {
                        id: area
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (key === "home" && activeController) activeController.openPath(homePath)
                            else if (key === "inspector") detailsVisible = !detailsVisible
                            else if (key === "terminal") terminalVisible = !terminalVisible
                            else if (key === "search") headerBar.focusSearchField()
                            else if (key === "refresh" && activeController) activeController.refresh()
                            else if (key === "new_folder" && centerWorkspace.activeView) centerWorkspace.activeView.openCreateFolderDialog()
                            else if (key === "new_tab_home") addTab(homePath)
                            commandPalette.close()
                        }
                    }
                }
            }
        }
    }
}
