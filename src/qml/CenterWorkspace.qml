import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Liphis.Core 1.0

SplitView {
    id: root

    property var theme
    property string homePath: ""
    property bool terminalVisible: false
    property var activeController: null
    property var activeView: null
    property var itemMenuHandler: null
    property bool splitVisible: false
    property string splitPath: ""
    property var docIntelController: null
    property var toastManager: null
    property var lastFileController: null

    signal controllerChanged(var controller)

    orientation: Qt.Vertical
    handle: Rectangle { implicitHeight: 1; color: theme.border }

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

    function addTab(path) {
        var normalized = normalizePath(path)
        if (!normalized || normalized.length === 0) return
        tabModel.append({"title": normalized.split('/').pop() || "Root", "path": normalized, "kind": "file", "section": ""})
        tabView.currentIndex = tabModel.count - 1
    }

    function openDocCenter(section) {
        var targetSection = section && section.length > 0 ? section : "inbox"
        for (var i = 0; i < tabModel.count; ++i) {
            if (tabModel.get(i).kind === "doc_center") {
                tabModel.setProperty(i, "section", targetSection)
                tabView.currentIndex = i
                return
            }
        }
        tabModel.append({
            "title": "Document Center",
            "path": "",
            "kind": "doc_center",
            "section": targetSection
        })
        tabView.currentIndex = tabModel.count - 1
    }

    function openSplit(path) {
        var normalized = normalizePath(path)
        if (!normalized || normalized.length === 0) return
        splitVisible = true
        splitPath = normalized
        if (secondaryView && secondaryView.controller) {
            secondaryView.controller.openPath(normalized)
        }
    }

    function closeSplit() {
        splitVisible = false
        splitPath = ""
    }

    function closeCurrentTab() {
        var idx = tabView.currentIndex
        if (idx >= 0 && idx < tabModel.count) {
            tabModel.remove(idx)
            if (tabModel.count === 0) {
                tabModel.append({"title": "Home", "path": root.homePath, "kind": "file", "section": ""})
                tabView.currentIndex = 0
            } else if (tabView.currentIndex >= tabModel.count) {
                tabView.currentIndex = tabModel.count - 1
            }
        }
    }

    function updateActiveController() {
        if (tabView.count > tabView.currentIndex && tabView.currentIndex >= 0) {
            var view = tabRepeater.itemAt(tabView.currentIndex)
            if (view) {
                if (view.kind === "doc_center") {
                    root.activeView = null
                    if (root.lastFileController && root.activeController !== root.lastFileController) {
                        root.activeController = root.lastFileController
                        root.controllerChanged(root.activeController)
                    }
                    return
                }

                var controllerRef = null
                if (view.item && view.item.controller !== undefined && view.item.controller !== null) {
                    root.activeView = view.item
                    controllerRef = view.item.controller
                } else if (view.controller !== undefined && view.controller !== null) {
                    root.activeView = view
                    controllerRef = view.controller
                }
                if (controllerRef !== null && controllerRef.openPath !== undefined) {
                    root.lastFileController = controllerRef
                    root.activeController = controllerRef
                    root.controllerChanged(root.activeController)
                }
            }
        }
    }

    Item {
        SplitView.fillHeight: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            TabStrip {
                id: tabs
                Layout.fillWidth: true
                theme: root.theme
                tabModel: tabModel
                currentIndex: tabView.currentIndex
                onTabSelected: (index) => tabView.currentIndex = index
                onTabClosed: (index) => {
                    tabModel.remove(index)
                    if (tabModel.count === 0) {
                        tabModel.append({"title": "Home", "path": root.homePath, "kind": "file", "section": ""})
                        tabView.currentIndex = 0
                    } else if (tabView.currentIndex >= tabModel.count) {
                        tabView.currentIndex = tabModel.count - 1
                    }
                }
                onTabMoved: (from, to) => {
                    if (from === to) return
                    tabModel.move(from, to, 1)
                    tabView.currentIndex = to
                }
                onHomeRequested: root.addTab(root.homePath)
            }

            SplitView {
                id: workspaceSplit
                Layout.fillWidth: true
                Layout.fillHeight: true
                orientation: Qt.Horizontal
                handle: Rectangle { implicitWidth: 1; color: theme.border }

                Item {
                    SplitView.fillWidth: true
                    SplitView.minimumWidth: 320

                    StackLayout {
                        id: tabView
                        anchors.fill: parent
                        onCurrentIndexChanged: root.updateActiveController()
                        onChildrenChanged: root.updateActiveController()

                        Repeater {
                            id: tabRepeater
                            model: tabModel

                            Loader {
                                active: true
                                sourceComponent: model.kind === "doc_center" ? docCenterComponent : fileViewComponent

                                property int tabIndex: index
                                property var tabModelData: model
                                property string kind: model.kind
                            }
                        }

                        Component {
                            id: fileViewComponent
                            FileView {
                                initialPath: parent.tabModelData ? parent.tabModelData.path : root.homePath
                                theme: root.theme
                                itemMenuHandler: root.itemMenuHandler
                                onRequestedActive: {
                                    tabView.currentIndex = parent.tabIndex !== undefined ? parent.tabIndex : -1
                                    Qt.callLater(focusFileArea)
                                }
                                onTabTitleChanged: (title) => {
                                    if (parent.tabIndex !== undefined && parent.tabIndex >= 0 && parent.tabIndex < tabModel.count && title && title.length > 0) {
                                        tabModel.setProperty(parent.tabIndex, "title", title)
                                    }
                                }
                            }
                        }

                        Component {
                            id: docCenterComponent
                            DocumentCenterPage {
                                theme: root.theme
                                controller: root.docIntelController
                                toastManager: root.toastManager
                                section: parent.tabModelData ? parent.tabModelData.section : ""
                            }
                        }
                    }
                }

                Item {
                    visible: root.splitVisible
                    SplitView.preferredWidth: Math.max(340, root.width * 0.38)
                    SplitView.minimumWidth: 320

                    Rectangle {
                        anchors.fill: parent
                        color: theme.surface
                        border.color: theme.border
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 34
                                color: theme.surfaceRaised
                                border.color: theme.border

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 6
                                    spacing: 8

                                    Text {
                                        Layout.fillWidth: true
                                        text: secondaryView && secondaryView.controller ? secondaryView.controller.title : "Split View"
                                        color: theme.textPrimary
                                        font.pixelSize: 12
                                        font.weight: Font.Medium
                                        elide: Text.ElideRight
                                    }

                                    ThemedIconButton {
                                        theme: root.theme
                                        iconName: "x"
                                        iconSize: 13
                                        toolTip: "Close Split View"
                                        onClicked: root.closeSplit()
                                    }
                                }
                            }

                            FileView {
                                id: secondaryView
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                initialPath: root.splitPath && root.splitPath.length > 0 ? root.splitPath : root.homePath
                                theme: root.theme
                                itemMenuHandler: root.itemMenuHandler
                            }
                        }
                    }
                }
            }

            ListModel {
            id: tabModel
            Component.onCompleted: append({"title": "Home", "path": root.normalizePath(root.homePath), "kind": "file", "section": ""})
        }
    }
    }

    // Terminal Panel
    Rectangle {
        id: terminalPanel
        SplitView.preferredHeight: 220
        SplitView.fillWidth: true
        visible: root.terminalVisible
        color: theme.surface
        
        TerminalManager {
            id: terminalManager
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // Terminal Toolbar
            Rectangle {
                Layout.fillWidth: true
                height: 32
                color: theme.surfaceRaised
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 12
                    Text { text: "TERMINAL - " + (terminalManager.currentDir || ""); color: theme.textSecondary; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1 }
                    Item { Layout.fillWidth: true }
                    ThemedIconButton { theme: root.theme; iconName: "trash"; iconSize: 12; onClicked: terminalManager.clear() }
                }
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }
            }

            ScrollView {
                id: terminalScroll
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                
                TextArea {
                    id: terminalOutput
                    text: terminalManager.output
                    readOnly: true
                    font.family: "Monospace"
                    font.pixelSize: 12
                    color: theme.success
                    background: null
                    wrapMode: TextArea.Wrap
                    padding: 12
                    onTextChanged: {
                        terminalScroll.ScrollBar.vertical.position = 1.0
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 36
                color: theme.surfaceMuted
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    spacing: 8
                    Text { text: "$"; color: theme.accent; font.family: "Monospace"; font.bold: true }
                    TextField {
                        id: terminalInput
                        Layout.fillWidth: true
                        font.family: "Monospace"
                        font.pixelSize: 12
                        color: theme.textPrimary
                        background: null
                        placeholderText: "Enter command..."
                        selectByMouse: true
                        onAccepted: {
                            if (text.trim().length > 0) {
                                terminalManager.sendCommand(text)
                                text = ""
                            }
                        }
                    }
                }
            }
        }

        Connections {
            target: root
            function onActiveControllerChanged() {
                if (root.activeController && root.activeController.currentPath) {
                    terminalManager.currentDir = root.activeController.currentPath
                }
            }
        }
    }
}
