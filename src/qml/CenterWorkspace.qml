import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
        tabModel.append({"title": normalized.split('/').pop() || "Root", "path": normalized})
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

    function updateActiveController() {
        if (tabView.count > tabView.currentIndex && tabView.currentIndex >= 0) {
            var view = tabRepeater.itemAt(tabView.currentIndex)
            if (view) {
                root.activeView = view
                root.activeController = view.controller
                root.controllerChanged(root.activeController)
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
                        tabModel.append({"title": "Home", "path": root.homePath})
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

                            FileView {
                                initialPath: model.path
                                theme: root.theme
                                itemMenuHandler: root.itemMenuHandler
                                onRequestedActive: tabView.currentIndex = index
                                onTabTitleChanged: (title) => {
                                    if (index >= 0 && index < tabModel.count && title && title.length > 0) {
                                        tabModel.setProperty(index, "title", title)
                                    }
                                }
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

                        FileView {
                            id: secondaryView
                            anchors.fill: parent
                            initialPath: root.splitPath && root.splitPath.length > 0 ? root.splitPath : root.homePath
                            theme: root.theme
                            itemMenuHandler: root.itemMenuHandler
                        }
                    }
                }
            }

            ListModel {
                id: tabModel
                Component.onCompleted: append({"title": "Home", "path": root.normalizePath(root.homePath)})
            }
        }
    }

    Rectangle {
        SplitView.preferredHeight: 220
        SplitView.fillWidth: true
        visible: root.terminalVisible
        color: theme.surface

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                height: 32
                color: theme.surfaceRaised

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: theme ? theme.space16 : 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: "TERMINAL"
                    color: theme.textSecondary
                    font.pixelSize: theme ? theme.fontLabel : 10
                    font.bold: true
                    font.letterSpacing: theme ? theme.letterSpacingLabel : 1
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: theme.border
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextArea {
                    readOnly: true
                    text: "liphis@linux:~$ _"
                    font.family: "Monospace"
                    font.pixelSize: 13
                    color: theme.success
                    background: null
                    padding: theme ? theme.space12 : 12
                }
            }
        }
    }
}
