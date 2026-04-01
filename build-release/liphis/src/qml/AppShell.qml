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
    property alias toastManager: toastManager
    property alias progressDialog: progressDialog
    property alias bulkRenameDialog: bulkRenameDialog
    property alias connectRemoteDialog: connectRemoteDialog
    property alias settingsDialog: settingsDialog
    property var viewSettings: settingsDialog.viewSettings
    property var generalSettings: settingsDialog.generalSettings
    property bool sidebarExpanded: true
    property string homePath: resolveHomePath()

    Settings {
        id: uiSettings
        category: "ui"
        property bool darkMode: false
        property bool detailsPanelVisible: true
        property bool terminalPanelVisible: false
        property bool sidebarExpanded: true
        property int windowWidth: 1200
        property int windowHeight: 800
    }

    Component.onCompleted: {
        appWindow.width = Math.max(960, uiSettings.windowWidth)
        appWindow.height = Math.max(640, uiSettings.windowHeight)
        appWindow.theme.isDark = uiSettings.darkMode
        appWindow.detailsVisible = uiSettings.detailsPanelVisible
        appWindow.terminalVisible = uiSettings.terminalPanelVisible
        appWindow.sidebarExpanded = uiSettings.sidebarExpanded

        // Register Commands
        // View & Navigation
        commandManager.registerCommand("view.grid", "View", "Switch to Grid View", "layout-grid", "Ctrl+1", function() { if (activeController) activeController.viewMode = "grid" })
        commandManager.registerCommand("view.list", "View", "Switch to List View", "layout-list", "Ctrl+2", function() { if (activeController) activeController.viewMode = "list" })
        commandManager.registerCommand("view.tree", "View", "Switch to Tree View", "layout-tree", "Ctrl+3", function() { if (activeController) activeController.viewMode = "tree" })
        commandManager.registerCommand("view.zoom_in", "View", "Zoom In", "zoom-in", "Ctrl++", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.zoomIn() })
        commandManager.registerCommand("view.zoom_out", "View", "Zoom Out", "zoom-out", "Ctrl+-", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.zoomOut() })
        commandManager.registerCommand("view.zoom_reset", "View", "Reset Zoom", "zoom-reset", "Ctrl+0", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.resetZoom() })
        commandManager.registerCommand("view.toggle_hidden", "View", "Toggle Hidden Files", "eye", "Ctrl+H", function() { if (activeController) activeController.showHiddenFiles = !activeController.showHiddenFiles })
        
        // Sorting
        commandManager.registerCommand("sort.name", "Sort", "Sort by Name", "alphabet-latin", "", function() { if (activeController) activeController.fileModel.setSortBy("name") })
        commandManager.registerCommand("sort.date", "Sort", "Sort by Date Modified", "calendar", "", function() { if (activeController) activeController.fileModel.setSortBy("date") })
        commandManager.registerCommand("sort.size", "Sort", "Sort by Size", "database", "", function() { if (activeController) activeController.fileModel.setSortBy("size") })

        // UI Customization
        commandManager.registerCommand("ui.toggle_sidebar", "UI", "Toggle Sidebar", "layout-sidebar", "Ctrl+B", function() { appWindow.sidebarExpanded = !appWindow.sidebarExpanded })
        commandManager.registerCommand("ui.toggle_inspector", "UI", "Toggle Inspector Panel", "info-circle", "Ctrl+I", function() { appWindow.detailsVisible = !appWindow.detailsVisible })
        commandManager.registerCommand("ui.toggle_terminal", "UI", "Toggle Terminal", "terminal", "Ctrl+`", function() { appWindow.terminalVisible = !appWindow.terminalVisible })
        commandManager.registerCommand("ui.toggle_theme", "UI", "Toggle Dark/Light Mode", "background", "", function() { appWindow.theme.isDark = !appWindow.theme.isDark })
        commandManager.registerCommand("ui.zen_mode", "UI", "Toggle Zen Mode", "app-window", "", function() { 
            appWindow.sidebarExpanded = !appWindow.sidebarExpanded;
            appWindow.detailsVisible = !appWindow.detailsVisible;
            appWindow.terminalVisible = false;
        })

        // File Operations
        commandManager.registerCommand("file.new_tab", "File", "New Tab", "tab-new", "Ctrl+T", function() { appWindow.addTab(appWindow.homePath) })
        commandManager.registerCommand("file.close_tab", "File", "Close Current Tab", "window-close", "Ctrl+W", function() { if (centerWorkspace.activeView) centerWorkspace.closeCurrentTab() })
        commandManager.registerCommand("file.new_folder", "File", "Create New Folder", "folder-plus", "Ctrl+Shift+N", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.openCreateFolderDialog() })
        commandManager.registerCommand("file.connect_server", "File", "Connect to Server", "server", "Ctrl+K", function() { connectRemoteDialog.open() })
        commandManager.registerCommand("file.refresh", "File", "Refresh Current Folder", "refresh", "Ctrl+R", function() { if (activeController) activeController.refresh() })
        commandManager.registerCommand("file.terminal", "File", "Open Terminal Here", "terminal", "", function() { if (activeController) activeController.openInTerminal(activeController.currentPath) })
        commandManager.registerCommand("file.code", "File", "Open in VS Code", "brand-vscode", "", function() { if (activeController) activeController.openInCode(activeController.currentPath) })
        commandManager.registerCommand("file.copy_path", "File", "Copy Path to Clipboard", "copy", "", function() { if (activeController) activeController.copyToClipboard(activeController.currentPath) })
        commandManager.registerCommand("file.select_all", "File", "Select All", "select-all", "Ctrl+A", function() { if (activeController) activeController.selectAll() })
        commandManager.registerCommand("file.clear_selection", "File", "Clear Selection", "x", "", function() { if (activeController) activeController.clearSelection() })
        commandManager.registerCommand("file.paste", "File", "Paste from Clipboard", "clipboard", "Ctrl+V", function() { if (activeController) activeController.pasteItem() })
        commandManager.registerCommand("file.undo", "File", "Undo last action", "arrow-back-up", "Ctrl+Z", function() { 
            if (activeController && activeController.canUndo) {
                var desc = activeController.undoDescription
                activeController.undo()
                toastManager.show("Undone: " + desc, false)
            }
        })
        commandManager.registerCommand("file.redo", "File", "Redo last action", "arrow-forward-up", "Ctrl+Y", function() { 
            if (activeController && activeController.canRedo) {
                activeController.redo()
                toastManager.show("Redone", false)
            }
        })
        
        // Navigation & Go
        commandManager.registerCommand("go.home", "Navigation", "Go to Home Directory", "home", "Alt+Home", function() { if (activeController) activeController.openPath(homePath) })
        commandManager.registerCommand("go.parent", "Navigation", "Go to Parent Directory", "arrow-up", "Alt+Up", function() { if (activeController) activeController.goUp() })
        commandManager.registerCommand("go.back", "Navigation", "Go Back", "arrow-left", "Alt+Left", function() { if (activeController) activeController.goBack() })
        commandManager.registerCommand("go.forward", "Navigation", "Go Forward", "arrow-right", "Alt+Right", function() { if (activeController) activeController.goForward() })
        commandManager.registerCommand("go.root", "Navigation", "Go to Root (/) ", "device-desktop", "", function() { if (activeController) activeController.openPath("/") })
        commandManager.registerCommand("go.downloads", "Navigation", "Go to Downloads", "download", "", function() { if (activeController) activeController.openPath(StandardPaths.writableLocation(StandardPaths.DownloadLocation)) })
        commandManager.registerCommand("go.documents", "Navigation", "Go to Documents", "file", "", function() { if (activeController) activeController.openPath(StandardPaths.writableLocation(StandardPaths.DocumentsLocation)) })
        commandManager.registerCommand("go.path", "Navigation", "Go to Path...", "search", "Ctrl+L", function() { headerBar.focusPathField() })
        
        // Help & System
        commandManager.registerCommand("help.shortcuts", "Help", "Keyboard Shortcuts", "info-circle", "Ctrl+?", function() { toastManager.show("Shortcuts: Ctrl+P: Palette, Ctrl+T: New Tab, Ctrl+W: Close Tab, Ctrl+L: Go to Path", false) })
        commandManager.registerCommand("help.categories", "Help", "Category Filter Help (@)", "info-circle", "", function() { toastManager.show("Type @ followed by category (e.g. @View) to filter commands", false) })
        commandManager.registerCommand("system.analyze", "System", "Analyze Disk Space", "chart-pie", "", function() { if (activeController) activeController.analyseFolder(activeController.currentPath) })
        commandManager.registerCommand("system.about", "System", "About Liphis", "info-circle", "", function() { toastManager.show("Liphis v0.1 - File Explorer", false) })
        commandManager.registerCommand("system.quit", "System", "Quit Liphis", "logout", "Ctrl+Q", function() { Qt.quit() })
    }

    onWidthChanged: uiSettings.windowWidth = width
    onHeightChanged: uiSettings.windowHeight = height
    onDetailsVisibleChanged: uiSettings.detailsPanelVisible = detailsVisible
    onTerminalVisibleChanged: uiSettings.terminalPanelVisible = terminalVisible
    onSidebarExpandedChanged: uiSettings.sidebarExpanded = sidebarExpanded
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

    Connections {
        target: activeController
        function onOpenWithRequested(path) { showOpenWith(path, activeController) }
        function onOperationError(msg) { toastManager.show(msg, true) }
        function onOperationSuccess(msg) { toastManager.show(msg, false) }
    }

    function showDirectoryMenu(path, controllerObj) {
        if (!path || !controllerObj) return
        bgMenu.controller = controllerObj
        bgMenu.targetPath = path
        bgMenu.popup()
    }

    function showOpenWith(path, controllerObj) {
        if (!path || !controllerObj) return
        openWithDialog.show(path, controllerObj)
    }

    function showBulkRename(paths) {
        if (!paths || paths.length === 0) return
        bulkRenameDialog.show(paths, activeController)
    }

    function showPropertiesForPath(path, controllerObj) {
        if (!path || !controllerObj) return
        var metadata = controllerObj.metadataForPath(path)
        if (Object.keys(metadata).length > 0) {
            // Ensure mimeType is available for PropertiesDialog
            if (!metadata.mimeType && !metadata.isDir) {
                metadata.mimeType = controllerObj.getMimeType(path)
            }
            propsDialog.show(metadata)
        }
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
        console.log("[DEBUG] activeController changed to:", activeController)
        homePath = resolveHomePath()
        if (activeController) {
            activeController.setThumbnailManager(globalThumbnailManager)
            activeController.setPlacesModel(globalPlacesModel)
            
            if (generalSettings) {
                activeController.showHiddenFiles = generalSettings.showHiddenByDefault
            }
            if (viewSettings) {
                activeController.viewMode = viewSettings.defaultViewMode
            }
            
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
        onSettingsRequested: settingsDialog.open()
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
        sequence: "Ctrl+P"
        onActivated: {
            commandPalette.open()
        }
    }

    Shortcut {
        id: globalEscapeShortcut
        sequences: ["Escape", "Esc", "Meta"]
        context: Qt.ApplicationShortcut
        onActivated: {
            console.log("[DEBUG] globalEscapeShortcut onActivated! Focused item:", appWindow.activeFocusItem)
            
            // Priority 1: Dialogs
            if (settingsDialog.visible) { console.log("[DEBUG] Closing settings"); settingsDialog.close(); return; }
            if (propsDialog.visible) { console.log("[DEBUG] Closing props"); propsDialog.close(); return; }
            if (bulkRenameDialog.visible) { console.log("[DEBUG] Closing bulk rename"); bulkRenameDialog.close(); return; }
            if (openWithDialog.visible) { console.log("[DEBUG] Closing open with"); openWithDialog.close(); return; }
            if (connectRemoteDialog.visible) { console.log("[DEBUG] Closing remote"); connectRemoteDialog.close(); return; }
            if (analyseDialog.visible) { console.log("[DEBUG] Closing analysis"); analyseDialog.close(); return; }
            if (progressDialog.visible) { console.log("[DEBUG] Closing progress"); progressDialog.close(); return; }

            // Priority 2: Command Palette
            if (commandPalette.opened) {
                console.log("[DEBUG] Closing command palette")
                commandPalette.close()
                return
            }

            // Priority 3: Search
            if (headerBar.hasActiveSearch()) {
                console.log("[DEBUG] Clearing search")
                headerBar.clearSearchAndRestore()
                return
            }

            // Priority 4: Selection
            if (activeController && activeController.hasSelection) {
                activeController.clearSelection()
                toastManager.show("Selection cleared", false)
            }
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
                Layout.preferredWidth: appWindow.sidebarExpanded ? theme.sidebarWidth : 64
                activeController: appWindow.activeController
                Behavior on Layout.preferredWidth {
                    NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
                }
                Layout.fillHeight: true
                theme: appWindow.theme
                placesModel: globalPlacesModel
                activePath: activeController ? activeController.currentPath : ""
                homePath: appWindow.homePath
                expanded: appWindow.sidebarExpanded
                onToggleExpanded: appWindow.sidebarExpanded = !appWindow.sidebarExpanded
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
                    SplitView.preferredWidth: theme.inspectorWidth
                    SplitView.minimumWidth: 220
                    SplitView.maximumWidth: 560
                    theme: appWindow.theme
                    activeController: appWindow.activeController
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
        activeController: appWindow.activeController
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

    DirectoryContextMenu {
        id: bgMenu
        darkMode: theme.isDark
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

    CommandPalette {
        id: commandPalette
        theme: appWindow.theme
    }

    SettingsDialog {
        id: settingsDialog
        theme: appWindow.theme
    }

    BulkRenameDialog {
        id: bulkRenameDialog
        theme: appWindow.theme
    }

    ConnectRemoteDialog {
        id: connectRemoteDialog
        onConnectRequested: (url) => { if (activeController) activeController.connectRemote(url) }
    }

    OpenWithDialog {
        id: openWithDialog
        theme: appWindow.theme
    }

    ProgressDialog {
        id: progressDialog
        theme: appWindow.theme
    }
}
