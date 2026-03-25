import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import liphis
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
    
    background: Rectangle {
        color: theme.bg
    }
    
    function addTab(path) {
        tabView.addTab(path);
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
    
    property var activeController: null
    property bool detailsVisible: true
    property bool terminalVisible: false

    function updateActiveController() {
        if (tabView.count > tabView.currentIndex && tabView.currentIndex >= 0) {
            var view = tabRepeater.itemAt(tabView.currentIndex);
            if (view) {
                activeController = view.controller;
                if (activeController) {
                    activeController.setThumbnailManager(globalThumbnailManager);
                    activeController.setPlacesModel(globalPlacesModel);
                }
            }
        }
    }

    onActiveControllerChanged: {
        if (activeController) {
            activeController.refresh();
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

    // --- Header ---
    header: Rectangle {
        height: 56; color: theme.headerBg
        RowLayout {
            anchors.fill: parent; anchors.leftMargin: theme.spacingMedium; anchors.rightMargin: theme.spacingMedium; spacing: theme.spacingMedium
            
            // Logo & Title
            RowLayout {
                spacing: theme.spacingSmall
                Image { source: "../../LiphisApp-Logo.png"; Layout.preferredWidth: 22; Layout.preferredHeight: 22; smooth: true; opacity: 0.85 }
                Text { text: "LIPHIS"; color: theme.textPrimary; font.pixelSize: 13; font.bold: true; font.letterSpacing: 1.5; opacity: 0.9 }
            }
            
            Item { Layout.preferredWidth: theme.spacingLarge }
            
            // Navigation Controls
            RowLayout {
                spacing: 2
                ToolButton { 
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    enabled: activeController && activeController.canGoBack
                    contentItem: Icon { name: "back"; size: 16; color: parent.enabled ? theme.textPrimary : theme.textMuted }
                    onClicked: activeController.goBack()
                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                }
                ToolButton { 
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    enabled: activeController && activeController.canGoForward
                    contentItem: Icon { name: "forward"; size: 16; color: parent.enabled ? theme.textPrimary : theme.textMuted }
                    onClicked: activeController.goForward()
                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                }
                ToolButton { 
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    contentItem: Icon { name: "up"; size: 16; color: theme.textPrimary }
                    onClicked: if(activeController) activeController.goUp()
                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                }
                ToolButton { 
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    contentItem: Icon { name: "refresh"; size: 16; color: theme.textPrimary }
                    onClicked: if(activeController) activeController.refresh()
                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                }
            }

            // Path Breadcrumb / Input
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 34; radius: theme.radius; color: theme.surfaceElevated; border.color: theme.border; border.width: 1
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 4; spacing: 8
                    Icon { name: "folder"; size: 14; color: theme.textSecondary }
                    TextInput {
                        Layout.fillWidth: true; color: theme.textPrimary; font.pixelSize: 12; verticalAlignment: Text.AlignVCenter
                        text: activeController ? activeController.currentPath : ""
                        onAccepted: if(activeController) activeController.openPath(text)
                        selectByMouse: true; selectionColor: theme.selection
                    }
                    ToolButton { 
                        Layout.preferredWidth: 28; Layout.preferredHeight: 28
                        contentItem: Icon { name: "star"; size: 14; color: theme.textSecondary }
                        onClicked: if(activeController) activeController.addToBookmarks(activeController.currentPath, activeController.title)
                        background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                    }
                }
            }

            // Search
            Rectangle {
                Layout.preferredWidth: 240; Layout.preferredHeight: 34; radius: theme.radius; color: theme.surfaceElevated; border.color: theme.border; border.width: 1
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12; spacing: 8
                    Icon { name: "search"; size: 14; color: theme.textSecondary }
                    TextField {
                        id: searchField; Layout.fillWidth: true; placeholderText: "Search..."; color: theme.textPrimary; font.pixelSize: 12; padding: 0
                        background: null
                        onTextChanged: if(activeController) activeController.setSearchText(text)
                        onAccepted: if(activeController) activeController.startGlobalSearch(text)
                    }
                }
            }

            // Actions
            RowLayout {
                spacing: 2
                ToolButton { 
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    checked: detailsVisible; checkable: true
                    contentItem: Icon { name: "info"; size: 16; color: parent.checked ? theme.accent : theme.textPrimary }
                    onClicked: detailsVisible = !detailsVisible
                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                }
                ToolButton { 
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    checked: terminalVisible; checkable: true
                    contentItem: Icon { name: "terminal"; size: 16; color: parent.checked ? theme.accent : theme.textPrimary }
                    onClicked: terminalVisible = !terminalVisible
                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                }
            }
        }
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }
    }

    ColumnLayout {
        anchors.fill: parent; spacing: 0

        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0
            
            // --- Left Sidebar ---
            Rectangle {
                id: sideBar; Layout.preferredWidth: 240; Layout.fillHeight: true; color: theme.sidebarBg
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: theme.spacingMedium; spacing: 8
                    Text { 
                        text: "PLACES"; color: theme.textMuted; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.2
                        Layout.topMargin: 8; Layout.bottomMargin: 8; Layout.leftMargin: 12
                    }
                    ListView {
                        id: sideCtrl; Layout.fillWidth: true; Layout.fillHeight: true; model: globalPlacesModel; spacing: 2
                        property string homePath: activeController ? activeController.homePath : ""
                        delegate: ItemDelegate {
                            width: sideCtrl.width; height: 34; padding: 0
                            background: Rectangle { 
                                anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8
                                radius: theme.radiusSmall; color: (activeController && activeController.currentPath === model.path) ? theme.selection : (parent.hovered ? theme.hover : "transparent") 
                            }
                            contentItem: RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 16; spacing: 12
                                Icon { name: model.icon; size: 16; color: (activeController && activeController.currentPath === model.path) ? theme.accent : theme.textSecondary }
                                Text { text: model.name; color: (activeController && activeController.currentPath === model.path) ? theme.accent : theme.textPrimary; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                            }
                            onClicked: if(activeController) activeController.openPath(model.path)
                        }
                    }
                }
                Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: theme.border }
            }

            SplitView {
                Layout.fillWidth: true; Layout.fillHeight: true; orientation: Qt.Horizontal
                handle: Rectangle { implicitWidth: 1; color: theme.border }

                SplitView {
                    SplitView.fillWidth: true; orientation: Qt.Vertical
                    handle: Rectangle { implicitHeight: 1; color: theme.border }
                    
                    // --- Main Content Area ---
                    ColumnLayout {
                        SplitView.fillHeight: true; spacing: 0
                        Rectangle {
                            Layout.fillWidth: true; height: 40; color: theme.surfaceMain
                            RowLayout {
                                anchors.fill: parent; spacing: 0
                                ListView {
                                    id: tabList; Layout.fillWidth: true; Layout.fillHeight: true; orientation: ListView.Horizontal; model: tabModel; spacing: 2
                                    delegate: Rectangle {
                                        width: Math.min(180, tabList.width / tabModel.count); height: 40
                                        color: tabView.currentIndex === index ? theme.surfaceElevated : "transparent"
                                        radius: theme.radiusSmall
                                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 2; color: theme.accent; visible: tabView.currentIndex === index }
                                        Text { anchors.centerIn: parent; text: model.title; color: tabView.currentIndex === index ? theme.textPrimary : theme.textSecondary; font.pixelSize: 12; elide: Text.ElideRight; width: parent.width - 40 }
                                        MouseArea { anchors.fill: parent; onClicked: { tabView.currentIndex = index; } }
                                        ToolButton { 
                                            anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; width: 24; height: 24; flat: true
                                            contentItem: Icon { name: "close"; size: 12; color: theme.textMuted }
                                            onClicked: { tabModel.remove(index); } 
                                            background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                                        }
                                    }
                                }
                                ToolButton { 
                                    Layout.preferredWidth: 40; Layout.preferredHeight: 40
                                    contentItem: Icon { name: "home"; size: 16; color: theme.textSecondary }
                                    onClicked: { tabView.addTab(sideCtrl.homePath); } 
                                    background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                                }
                            }
                            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }
                        }
                        
                        StackLayout {
                            id: tabView; Layout.fillWidth: true; Layout.fillHeight: true
                            onCurrentIndexChanged: updateActiveController()
                            onChildrenChanged: updateActiveController()
                            function addTab(path) { tabModel.append({"title": path.split('/').pop() || "Root", "path": path}); currentIndex = tabModel.count - 1 }
                            Repeater { 
                                id: tabRepeater; model: tabModel
                                FileView { 
                                    initialPath: model.path; theme: appWindow.theme
                                    onRequestedActive: { tabView.currentIndex = index; } 
                                } 
                            }
                        }
                        ListModel { id: tabModel; Component.onCompleted: { append({"title": "Home", "path": sideCtrl.homePath}); } }
                    }
                    
                    // --- Terminal Area ---
                    Rectangle {
                        SplitView.preferredHeight: 220; SplitView.fillWidth: true; visible: terminalVisible; color: theme.surfaceMain
                        ColumnLayout { 
                            anchors.fill: parent; spacing: 0
                            Rectangle { 
                                Layout.fillWidth: true; height: 32; color: theme.surfaceElevated
                                Text { anchors.left: parent.left; anchors.leftMargin: 16; anchors.verticalCenter: parent.verticalCenter; text: "TERMINAL"; color: theme.textSecondary; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1 } 
                                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }
                            }
                            ScrollView { 
                                Layout.fillWidth: true; Layout.fillHeight: true
                                TextArea { readOnly: true; text: "liphis@linux:~$ _"; font.family: "Monospace"; font.pixelSize: 13; color: theme.success; background: null; padding: 12 } 
                            }
                        }
                    }
                }

                // --- Right Sidebar / Details Panel ---
                Rectangle {
                    id: detailsPanel; SplitView.preferredWidth: 300; SplitView.minimumWidth: 260; SplitView.maximumWidth: 500; color: theme.surfaceElevated; visible: detailsVisible
                    property var meta: (activeController && activeController.hasSelection) ? activeController.metadataForPath(activeController.selectedPath) : null
                    function formatSize(b) { if(!b) return "0 B"; var units=["B","KB","MB","GB","TB"]; var i=0; while(b>=1024){b/=1024; i++} return b.toFixed(1)+" "+units[i] }
                    function formatDate(s) { if(!s) return "--"; return new Date(s*1000).toLocaleString() }
                    
                    ScrollView {
                        anchors.fill: parent; contentWidth: availableWidth; clip: true
                        ColumnLayout {
                            width: detailsPanel.width; anchors.margins: 24; spacing: 24; visible: !!detailsPanel.meta
                            
                            Rectangle { 
                                Layout.fillWidth: true; Layout.preferredHeight: Math.min(200, parent.width * 0.8); radius: theme.radiusLarge; color: theme.surfaceSecondary; border.color: theme.border; border.width: 1; clip: true
                                Image { 
                                    anchors.fill: parent; fillMode: Image.PreserveAspectFit; anchors.margins: 16; source: (detailsPanel.meta && detailsPanel.meta.thumbnail) ? detailsPanel.meta.thumbnail : ""; asynchronous: true
                                    FileIcon { anchors.centerIn: parent; width: 64; height: 64; isDir: !!(detailsPanel.meta && detailsPanel.meta.isDir); visible: parent.status !== Image.Ready } 
                                }
                            }
                            
                            ColumnLayout { 
                                spacing: 6; Layout.fillWidth: true
                                Text { text: detailsPanel.meta ? detailsPanel.meta.name : ""; color: theme.textPrimary; font.pixelSize: 16; font.bold: true; elide: Text.ElideMiddle; Layout.fillWidth: true; horizontalAlignment: Text.AlignLeft }
                                Text { text: detailsPanel.meta ? (detailsPanel.meta.isDir ? "Folder" : detailsPanel.meta.mimeType) : ""; color: theme.textSecondary; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true; horizontalAlignment: Text.AlignLeft } 
                            }
                            
                            Rectangle { Layout.fillWidth: true; height: 1; color: theme.border }
                            
                            GridLayout {
                                columns: 2; rowSpacing: 16; columnSpacing: 16; Layout.fillWidth: true
                                Text { text: "Size"; color: theme.textMuted; font.pixelSize: 11; Layout.preferredWidth: 80 }
                                Text { text: detailsPanel.meta ? (detailsPanel.meta.isDir ? "Folder" : detailsPanel.formatSize(detailsPanel.meta.size)) : ""; color: theme.textPrimary; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                                
                                Text { text: "Modified"; color: theme.textMuted; font.pixelSize: 11 }
                                Text { text: detailsPanel.meta ? detailsPanel.formatDate(detailsPanel.meta.mtime) : ""; color: theme.textPrimary; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                                
                                Text { text: "Permissions"; color: theme.textMuted; font.pixelSize: 11 }
                                Text { text: detailsPanel.meta ? detailsPanel.meta.permissions : ""; color: theme.textPrimary; font.family: "Monospace"; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                                
                                Text { text: "Owner"; color: theme.textMuted; font.pixelSize: 11 }
                                Text { text: (detailsPanel.meta ? (detailsPanel.meta.owner + ":" + detailsPanel.meta.group) : ""); color: theme.textPrimary; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                            }
                            
                            Button { 
                                id: analyseBtn; visible: !!(detailsPanel.meta && detailsPanel.meta.isDir); Layout.fillWidth: true; implicitHeight: 40; text: "Analyse Disk Usage"
                                onClicked: { if(activeController) activeController.analyseFolder(detailsPanel.meta.path); }
                                background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.selection : theme.surfaceSecondary; border.color: theme.border }
                                contentItem: RowLayout { 
                                    anchors.centerIn: parent; spacing: 8
                                    Icon { name: "info"; size: 14; color: theme.accent }
                                    Text { text: analyseBtn.text; color: theme.textPrimary; font.pixelSize: 12; font.weight: Font.Medium } 
                                } 
                            }
                        }
                    }
                }
            }
        }

        // --- Status Bar ---
        Rectangle {
            Layout.fillWidth: true; height: 32; color: theme.surfaceElevated; border.color: theme.border
            Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: theme.border }
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16
                Text { text: (activeController && activeController.loading) ? "Scanning..." : "Ready"; color: theme.textSecondary; font.pixelSize: 11 }
                Item { Layout.fillWidth: true }
                ListView { 
                    id: filterList; Layout.fillWidth: true; Layout.fillHeight: true; orientation: ListView.Horizontal; spacing: 8; clip: true; model: (activeController && activeController.availableExtensions) ? activeController.availableExtensions : []
                    delegate: Button { 
                        anchors.verticalCenter: parent ? parent.verticalCenter : undefined; text: modelData; flat: true; font.pixelSize: 11; implicitHeight: 24
                        onClicked: { if(activeController) activeController.fileModel.extensionFilter = modelData; } 
                        contentItem: Text { text: parent.text; color: theme.textSecondary; font.pixelSize: 11; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: theme.radiusSmall; color: parent.hovered ? theme.hover : "transparent" }
                    } 
                }
            }
        }
    }
    PropertiesDialog {
        id: propsDialog
        onChangePermissionsRequested: (path, newPerms) => {
            if (activeController) activeController.setPermissions(path, newPerms)
        }
    }

    AnalyseDialog {
        id: analyseDialog
        analysisController: analysisController
        toastManager: toastManager
    }

    FileContextMenu {
        id: fileMenu
        propsDialog: propsDialog
        toastManager: toastManager
    }

    FolderContextMenu {
        id: folderMenu
        propsDialog: propsDialog
        onOpenInNewTab: (path) => appWindow.addTab(path)
        onOpenInNewWindow: (_path) => toastManager.show("Open in new window is not implemented yet", true)
        onOpenInSplitView: (_path) => toastManager.show("Split view is not implemented yet", true)
    }

    MediaContextMenu {
        id: mediaMenu
        propsDialog: propsDialog
        toastManager: toastManager
    }

    ToastManager { id: toastManager }
}
