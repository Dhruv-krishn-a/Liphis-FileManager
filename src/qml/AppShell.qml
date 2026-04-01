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
    property bool docIntelEnabled: true
    property bool docIntelReady: false
    property var hotkeyCatalog: ({})
    property var hotkeyEntries: []
    property var hotkeyOverrides: ({})

    Settings {
        id: uiSettings
        category: "ui"
        property bool darkMode: false
        property bool detailsPanelVisible: true
        property bool terminalPanelVisible: false
        property bool sidebarExpanded: true
        property int windowWidth: 1200
        property int windowHeight: 800
        property bool docIntelEnabled: true
    }

    Settings {
        id: hotkeySettings
        category: "hotkeys"
        property string customBindings: "{}"
    }

    function loadHotkeyOverrides() {
        try {
            const parsed = JSON.parse(hotkeySettings.customBindings || "{}")
            hotkeyOverrides = parsed && typeof parsed === "object" ? parsed : {}
        } catch (e) {
            hotkeyOverrides = {}
        }
    }

    function saveHotkeyOverrides() {
        hotkeySettings.customBindings = JSON.stringify(hotkeyOverrides)
    }

    function resolvedShortcut(commandId, defaultShortcut) {
        if (hotkeyOverrides.hasOwnProperty(commandId)) return hotkeyOverrides[commandId]
        return defaultShortcut || ""
    }

    function setCommandShortcut(commandId, shortcut) {
        const raw = (shortcut || "").trim()
        const next = commandManager.normalizeShortcut(raw)

        if (raw.length > 0 && next.length === 0) {
            if (toastManager) toastManager.show("Invalid shortcut format", true)
            return false
        }

        if (next.length > 0) {
            for (const id in hotkeyCatalog) {
                if (id === commandId) continue
                const existing = resolvedShortcut(id, hotkeyCatalog[id].defaultShortcut)
                if (existing.length > 0 && existing.toLowerCase() === next.toLowerCase()) {
                    hotkeyOverrides[id] = ""
                    if (toastManager) {
                        toastManager.show("Shortcut " + next + " moved from " + hotkeyCatalog[id].label + " to " + hotkeyCatalog[commandId].label, false)
                    }
                    commandManager.updateCommandShortcut(id, "")
                }
            }
        }

        hotkeyOverrides[commandId] = next
        saveHotkeyOverrides()
        applyShortcutToCommand(commandId)
        if (toastManager) toastManager.show("Shortcut updated", false)
        return true
    }

    function removeCommandShortcutOverride(commandId) {
        if (hotkeyOverrides.hasOwnProperty(commandId)) {
            delete hotkeyOverrides[commandId]
            saveHotkeyOverrides()
        }
        applyShortcutToCommand(commandId)
    }

    function resetAllCommandShortcuts() {
        hotkeyOverrides = {}
        saveHotkeyOverrides()
        for (const id in hotkeyCatalog) {
            applyShortcutToCommand(id)
        }
        refreshHotkeyEntries()
    }

    function applyShortcutToCommand(commandId) {
        if (!hotkeyCatalog[commandId]) return
        const seq = resolvedShortcut(commandId, hotkeyCatalog[commandId].defaultShortcut)
        commandManager.updateCommandShortcut(commandId, seq)
        refreshHotkeyEntries()
    }

    function registerHotkeyMeta(commandId, category, label, defaultShortcut) {
        hotkeyCatalog[commandId] = {
            id: commandId,
            category: category,
            label: label,
            defaultShortcut: defaultShortcut || ""
        }
    }

    function refreshHotkeyEntries() {
        const arr = []
        for (const id in hotkeyCatalog) {
            const def = hotkeyCatalog[id]
            arr.push({
                id: id,
                category: def.category,
                label: def.label,
                defaultShortcut: def.defaultShortcut,
                shortcut: resolvedShortcut(id, def.defaultShortcut),
                customized: hotkeyOverrides.hasOwnProperty(id)
            })
        }
        arr.sort((a, b) => {
            if (a.category !== b.category) return a.category.localeCompare(b.category)
            return a.label.localeCompare(b.label)
        })
        hotkeyEntries = arr
    }

    function shouldSkipGlobalHotkeys() {
        let item = appWindow.activeFocusItem
        while (item) {
            if (item.hasOwnProperty("cursorPosition") || item.hasOwnProperty("selectedText") || item.hasOwnProperty("echoMode")) {
                return true
            }
            item = item.parent
        }
        return false
    }

    function registerAppCommand(id, category, label, icon, defaultShortcut, action) {
        registerHotkeyMeta(id, category, label, defaultShortcut)
        commandManager.registerCommand(id, category, label, icon, resolvedShortcut(id, defaultShortcut), action)
    }

    function ensureDocIntelReady() {
        if (!docIntelEnabled || !docIntelController || docIntelReady) return
        docIntelController.initialize()
        docIntelReady = true
    }

    Component.onCompleted: {
        loadHotkeyOverrides()
        appWindow.width = Math.max(960, uiSettings.windowWidth)
        appWindow.height = Math.max(640, uiSettings.windowHeight)
        appWindow.theme.isDark = uiSettings.darkMode
        appWindow.detailsVisible = uiSettings.detailsPanelVisible
        appWindow.terminalVisible = uiSettings.terminalPanelVisible
        appWindow.sidebarExpanded = uiSettings.sidebarExpanded
        appWindow.docIntelEnabled = uiSettings.docIntelEnabled

        // Register Commands
        // View & Navigation
        registerAppCommand("view.grid", "View", "Switch to Grid View", "layout-grid", "Ctrl+1", function() { if (activeController) activeController.viewMode = "grid" })
        registerAppCommand("view.list", "View", "Switch to List View", "layout-list", "Ctrl+2", function() { if (activeController) activeController.viewMode = "list" })
        registerAppCommand("view.tree", "View", "Switch to Tree View", "layout-tree", "Ctrl+3", function() { if (activeController) activeController.viewMode = "tree" })
        registerAppCommand("view.zoom_in", "View", "Zoom In", "zoom-in", "Ctrl++", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.zoomIn() })
        registerAppCommand("view.zoom_out", "View", "Zoom Out", "zoom-out", "Ctrl+-", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.zoomOut() })
        registerAppCommand("view.zoom_reset", "View", "Reset Zoom", "zoom-reset", "Ctrl+0", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.resetZoom() })
        registerAppCommand("view.toggle_hidden", "View", "Toggle Hidden Files", "eye", "Ctrl+H", function() { if (activeController) activeController.showHiddenFiles = !activeController.showHiddenFiles })
        
        // Sorting
        registerAppCommand("sort.name", "Sort", "Sort by Name", "alphabet-latin", "", function() { if (activeController) activeController.fileModel.setSortBy("name") })
        registerAppCommand("sort.date", "Sort", "Sort by Date Modified", "calendar", "", function() { if (activeController) activeController.fileModel.setSortBy("date") })
        registerAppCommand("sort.size", "Sort", "Sort by Size", "database", "", function() { if (activeController) activeController.fileModel.setSortBy("size") })

        // UI Customization
        registerAppCommand("ui.toggle_sidebar", "UI", "Toggle Sidebar", "layout-sidebar", "Ctrl+B", function() { appWindow.sidebarExpanded = !appWindow.sidebarExpanded })
        registerAppCommand("ui.toggle_inspector", "UI", "Toggle Inspector Panel", "info-circle", "Ctrl+I", function() { appWindow.detailsVisible = !appWindow.detailsVisible })
        registerAppCommand("ui.toggle_terminal", "UI", "Toggle Terminal", "terminal", "Ctrl+`", function() { appWindow.terminalVisible = !appWindow.terminalVisible })
        registerAppCommand("ui.toggle_theme", "UI", "Toggle Dark/Light Mode", "background", "", function() { appWindow.theme.isDark = !appWindow.theme.isDark })
        registerAppCommand("ui.zen_mode", "UI", "Toggle Zen Mode", "app-window", "", function() { 
            appWindow.sidebarExpanded = !appWindow.sidebarExpanded;
            appWindow.detailsVisible = !appWindow.detailsVisible;
            appWindow.terminalVisible = false;
        })
        registerAppCommand("ui.open_palette", "UI", "Open Command Palette", "search", "Ctrl+P", function() { commandPalette.open() })
        registerAppCommand("search.focus", "Navigation", "Focus Search", "search", "Ctrl+F", function() { headerBar.focusSearchField() })

        // File Operations
        registerAppCommand("file.new_tab", "File", "New Tab", "tab-new", "Ctrl+T", function() { appWindow.addTab(appWindow.homePath) })
        registerAppCommand("file.close_tab", "File", "Close Current Tab", "window-close", "Ctrl+W", function() { if (centerWorkspace.activeView) centerWorkspace.closeCurrentTab() })
        registerAppCommand("file.new_folder", "File", "Create New Folder", "folder-plus", "Ctrl+Shift+N", function() { if (centerWorkspace.activeView) centerWorkspace.activeView.openCreateFolderDialog() })
        registerAppCommand("file.connect_server", "File", "Connect to Server", "server", "Ctrl+K", function() { connectRemoteDialog.open() })
        registerAppCommand("file.refresh", "File", "Refresh Current Folder", "refresh", "Ctrl+R", function() { if (activeController) activeController.refresh() })
        registerAppCommand("file.terminal", "File", "Open Terminal Here", "terminal", "", function() { if (activeController) activeController.openInTerminal(activeController.currentPath) })
        registerAppCommand("file.code", "File", "Open in VS Code", "brand-vscode", "", function() { if (activeController) activeController.openInCode(activeController.currentPath) })
        registerAppCommand("file.copy_path", "File", "Copy Path to Clipboard", "copy", "", function() { if (activeController) activeController.copyToClipboard(activeController.currentPath) })
        registerAppCommand("file.select_all", "File", "Select All", "select-all", "Ctrl+A", function() { if (activeController) activeController.selectAll() })
        registerAppCommand("file.clear_selection", "File", "Clear Selection", "x", "", function() { if (activeController) activeController.clearSelection() })
        registerAppCommand("file.paste", "File", "Paste from Clipboard", "clipboard", "Ctrl+V", function() { if (activeController) activeController.pasteItem() })
        registerAppCommand("file.undo", "File", "Undo last action", "arrow-back-up", "Ctrl+Z", function() { 
            if (activeController && activeController.canUndo) {
                var desc = activeController.undoDescription
                activeController.undo()
                toastManager.show("Undone: " + desc, false)
            }
        })
        registerAppCommand("file.redo", "File", "Redo last action", "arrow-forward-up", "Ctrl+Y", function() { 
            if (activeController && activeController.canRedo) {
                activeController.redo()
                toastManager.show("Redone", false)
            }
        })
        
        // Navigation & Go
        registerAppCommand("go.home", "Navigation", "Go to Home Directory", "home", "Alt+Home", function() { if (activeController) activeController.openPath(homePath) })
        registerAppCommand("go.parent", "Navigation", "Go to Parent Directory", "arrow-up", "Alt+Up", function() { if (activeController) activeController.goUp() })
        registerAppCommand("go.back", "Navigation", "Go Back", "arrow-left", "Alt+Left", function() { if (activeController) activeController.goBack() })
        registerAppCommand("go.forward", "Navigation", "Go Forward", "arrow-right", "Alt+Right", function() { if (activeController) activeController.goForward() })
        registerAppCommand("go.root", "Navigation", "Go to Root (/) ", "device-desktop", "", function() { if (activeController) activeController.openPath("/") })
        registerAppCommand("go.downloads", "Navigation", "Go to Downloads", "download", "", function() { if (activeController) activeController.openPath(StandardPaths.writableLocation(StandardPaths.DownloadLocation)) })
        registerAppCommand("go.documents", "Navigation", "Go to Documents", "file", "", function() { if (activeController) activeController.openPath(StandardPaths.writableLocation(StandardPaths.DocumentsLocation)) })
        registerAppCommand("go.path", "Navigation", "Go to Path...", "search", "Ctrl+L", function() { headerBar.focusPathField() })
        
        // Help & System
        registerAppCommand("help.shortcuts", "Help", "Keyboard Shortcuts", "info-circle", "Ctrl+?", function() { toastManager.show("Shortcuts are customizable in Settings > Hotkeys", false) })
        registerAppCommand("help.categories", "Help", "Category Filter Help (@)", "info-circle", "", function() { toastManager.show("Type @ followed by category (e.g. @View) to filter commands", false) })
        registerAppCommand("system.analyze", "System", "Analyze Disk Space", "chart-pie", "", function() { if (activeController) activeController.analyseFolder(activeController.currentPath) })
        registerAppCommand("system.about", "System", "About Liphis", "info-circle", "", function() { toastManager.show("Liphis v0.1 - File Explorer", false) })
        registerAppCommand("system.quit", "System", "Quit Liphis", "logout", "Ctrl+Q", function() { Qt.quit() })

        // Document Intelligence
        registerAppCommand("doc.scan_current", "Documents", "Index Documents In Current Folder", "search", "", function() {
            ensureDocIntelReady()
            if (docIntelController && activeController) docIntelController.scanFolder(activeController.currentPath)
        })
        registerAppCommand("doc.scan_downloads", "Documents", "Refresh Download Inbox", "download", "", function() {
            ensureDocIntelReady()
            if (docIntelController) docIntelController.scanDownloadsInbox()
        })
        registerAppCommand("doc.open_inbox", "Documents", "Open Document Inbox", "folder-open", "", function() {
            ensureDocIntelReady()
            centerWorkspace.openDocCenter("inbox")
        })
        registerAppCommand("doc.open_duplicates", "Documents", "Open Duplicates Center", "copy", "", function() {
            ensureDocIntelReady()
            centerWorkspace.openDocCenter("dupes")
        })
        registerAppCommand("doc.open_naming", "Documents", "Open Naming Assistant", "alphabet-latin", "", function() {
            if (canonicalNamingDialog) canonicalNamingDialog.open()
        })
        registerAppCommand("doc.open_health", "Documents", "Open Document Health Dashboard", "chart-pie", "", function() {
            ensureDocIntelReady()
            centerWorkspace.openDocCenter("health")
        })
        registerAppCommand("doc.toggle", "Documents", "Toggle Document Intelligence", "app-window", "", function() {
            appWindow.docIntelEnabled = !appWindow.docIntelEnabled
            if (docIntelController) docIntelController.enabled = appWindow.docIntelEnabled
            if (appWindow.docIntelEnabled) ensureDocIntelReady()
            toastManager.show("Document intelligence " + (appWindow.docIntelEnabled ? "enabled" : "disabled"), false)
        })

        refreshHotkeyEntries()
    }

    onWidthChanged: uiSettings.windowWidth = width
    onHeightChanged: uiSettings.windowHeight = height
    onDetailsVisibleChanged: uiSettings.detailsPanelVisible = detailsVisible
    onTerminalVisibleChanged: uiSettings.terminalPanelVisible = terminalVisible
    onSidebarExpandedChanged: uiSettings.sidebarExpanded = sidebarExpanded
    onDocIntelEnabledChanged: uiSettings.docIntelEnabled = docIntelEnabled
    Connections {
        target: appWindow.theme
        function onIsDarkChanged() { uiSettings.darkMode = appWindow.theme.isDark }
    }

    Connections {
        target: docIntelController
        function onOperationNotice(message, isError) {
            toastManager.show(message, isError)
        }
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
        homePath = resolveHomePath()
        if (activeController) {
            if (activeController.setThumbnailManager !== undefined) activeController.setThumbnailManager(globalThumbnailManager)
            if (activeController.setPlacesModel !== undefined) activeController.setPlacesModel(globalPlacesModel)
            
            if (generalSettings) {
                if (activeController.showHiddenFiles !== undefined) activeController.showHiddenFiles = generalSettings.showHiddenByDefault
            }
            if (viewSettings) {
                if (activeController.viewMode !== undefined) activeController.viewMode = viewSettings.defaultViewMode
            }
        }
    }

    Connections {
        target: (activeController && activeController.openPath !== undefined) ? activeController : null
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
        onAnalysisFinished: (result) => analyseDialog.finish(result)
    }

    DocumentIntelligenceController {
        id: docIntelController
        enabled: appWindow.docIntelEnabled
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

    Instantiator {
        id: userShortcutInstantiator
        model: appWindow.hotkeyEntries.filter((entry) => entry.shortcut && entry.shortcut.length > 0)
        delegate: Shortcut {
            required property var modelData
            sequence: modelData.shortcut
            context: Qt.ApplicationShortcut
            onActivated: {
                if (appWindow.shouldSkipGlobalHotkeys()) return
                commandManager.executeCommandById(modelData.id)
            }
        }
    }

    Shortcut {
        id: globalEscapeShortcut
        sequence: "Escape"
        context: Qt.ApplicationShortcut
        onActivated: {
            // Priority 1: Dialogs
            if (settingsDialog.visible) { settingsDialog.close(); return; }
            if (propsDialog.visible) { propsDialog.close(); return; }
            if (bulkRenameDialog.visible) { bulkRenameDialog.close(); return; }
            if (openWithDialog.visible) { openWithDialog.close(); return; }
            if (connectRemoteDialog.visible) { connectRemoteDialog.close(); return; }
            if (analyseDialog.visible) { analyseDialog.close(); return; }
            if (progressDialog.visible) { progressDialog.close(); return; }
            if (docInboxDialog.visible) { docInboxDialog.close(); return; }
            if (docDuplicatesDialog.visible) { docDuplicatesDialog.close(); return; }
            if (canonicalNamingDialog.visible) { canonicalNamingDialog.close(); return; }
            if (docHealthDialog.visible) { docHealthDialog.close(); return; }

            // Priority 2: Command Palette
            if (commandPalette.opened) {
                commandPalette.close()
                return
            }

            // Priority 3: Search
            if (headerBar.hasActiveSearch()) {
                headerBar.clearSearchAndRestore()
                return
            }

            // Priority 4: Selection
            const currentController = activeController ? activeController : (centerWorkspace ? centerWorkspace.activeController : null)
            if (currentController && currentController.hasSelection) {
                currentController.clearSelection()
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
                docIntelController: docIntelController
                activePath: (activeController && activeController.currentPath !== undefined) ? activeController.currentPath : ""
                homePath: appWindow.homePath
                expanded: appWindow.sidebarExpanded
                onToggleExpanded: appWindow.sidebarExpanded = !appWindow.sidebarExpanded
                onPathActivated: (path) => {
                    if (activeController) activeController.openPath(path)
                }
                onRemoveBookmarkRequested: (path) => {
                    if (activeController) activeController.removeBookmarkByPath(path)
                }
                onOpenDocInboxRequested: {
                    appWindow.ensureDocIntelReady()
                    centerWorkspace.openDocCenter("inbox")
                }
                onOpenDocDuplicatesRequested: {
                    appWindow.ensureDocIntelReady()
                    centerWorkspace.openDocCenter("dupes")
                }
                onOpenDocHealthRequested: {
                    appWindow.ensureDocIntelReady()
                    centerWorkspace.openDocCenter("health")
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
                    docIntelController: docIntelController
                    toastManager: toastManager
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
                    docIntelController: docIntelController
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
        hotkeyEntries: appWindow.hotkeyEntries
        setShortcutForCommand: (commandId, shortcut) => appWindow.setCommandShortcut(commandId, shortcut)
        clearShortcutOverride: (commandId) => appWindow.removeCommandShortcutOverride(commandId)
        resetAllShortcuts: () => appWindow.resetAllCommandShortcuts()
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

    DocumentInboxDialog {
        id: docInboxDialog
        theme: appWindow.theme
        controller: docIntelController
        toastManager: toastManager
    }

    DocumentDuplicatesDialog {
        id: docDuplicatesDialog
        theme: appWindow.theme
        controller: docIntelController
    }

    CanonicalNamingDialog {
        id: canonicalNamingDialog
        theme: appWindow.theme
        controller: docIntelController
        activeController: appWindow.activeController
        toastManager: toastManager
    }

    DocumentHealthDialog {
        id: docHealthDialog
        theme: appWindow.theme
        controller: docIntelController
        onOpenInboxRequested: docInboxDialog.open()
        onOpenDuplicatesRequested: docDuplicatesDialog.open()
        onOpenNamingRequested: canonicalNamingDialog.open()
    }
}
