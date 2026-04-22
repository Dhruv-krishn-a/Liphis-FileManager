import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Rectangle {
    id: root

    property var theme
    property var placesModel
    property string activePath: ""
    property string homePath: ""
    property bool expanded: true
    property var activeController: null
    property var docIntelController: null

    signal pathActivated(string path)
    signal removeBookmarkRequested(string path)
    signal toggleExpanded()
    signal openDocInboxRequested()
    signal openDocDuplicatesRequested()
    signal openDocHealthRequested()

    color: theme.sidebar

    Menu {
        id: sidebarItemMenu
        Material.theme: (theme && theme.isDark) ? Material.Dark : Material.Light
        Material.background: theme.surfaceElevated
        property string targetPath: ""
        property string targetName: ""

        MenuItem { 
            text: "Open"
            onTriggered: root.pathActivated(sidebarItemMenu.targetPath)
        }
        MenuItem { 
            text: "Open in New Tab"
            onTriggered: appWindow.addTab(sidebarItemMenu.targetPath)
        }
        MenuSeparator { visible: sidebarItemMenu.targetPath === "trash:///" }
        MenuItem {
            text: "Empty Trash"
            visible: sidebarItemMenu.targetPath === "trash:///"
            onTriggered: if (activeController) activeController.emptyTrash()
            icon.source: "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/trash.svg"
        }
    }

    Menu {
        id: sidebarBgMenu
        Material.theme: (theme && theme.isDark) ? Material.Dark : Material.Light
        Material.background: theme.surfaceElevated
        
        MenuItem { text: "Show Places"; checkable: true; checked: generalSettings.showPlaces; onTriggered: generalSettings.showPlaces = checked }
        MenuItem { text: "Show Bookmarks"; checkable: true; checked: generalSettings.showBookmarks; onTriggered: generalSettings.showBookmarks = checked }
        MenuItem { text: "Show Devices"; checkable: true; checked: generalSettings.showDevices; onTriggered: generalSettings.showDevices = checked }
        MenuItem { text: "Show Recent"; checkable: true; checked: generalSettings.showRecent; onTriggered: generalSettings.showRecent = checked }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        MouseArea {
            anchors.fill: parent
            z: -1
            acceptedButtons: Qt.RightButton
            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton) {
                    sidebarBgMenu.popup()
                }
            }
        }

        // Sidebar Header / Toggle
        Rectangle {
            Layout.fillWidth: true
            height: theme.headerHeight
            color: "transparent"

            ThemedIconButton {
                anchors.right: root.expanded ? parent.right : undefined
                anchors.rightMargin: root.expanded ? (theme ? theme.space8 : 8) : 0
                anchors.horizontalCenter: root.expanded ? undefined : parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                
                theme: root.theme
                iconName: root.expanded ? "layout-sidebar-left-collapse" : "layout-sidebar-left-expand"
                toolTip: root.expanded ? "Collapse Sidebar" : "Expand Sidebar"
                onClicked: root.toggleExpanded()
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: theme.border
                opacity: 0.3
            }
        }

        ListView {
            id: sideList
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: theme ? theme.space8 : 8
            Layout.leftMargin: root.expanded ? (theme ? theme.space12 : 12) : (theme ? theme.space8 : 8)
            Layout.rightMargin: root.expanded ? (theme ? theme.space12 : 12) : (theme ? theme.space8 : 8)
            model: root.placesModel
            spacing: theme ? theme.space4 : 4
            clip: true
            section.property: "category"
            section.criteria: ViewSection.FullString

            section.delegate: Rectangle {
                required property string section
                visible: {
                    if (section === "0") return generalSettings.showPlaces
                    if (section === "1") return generalSettings.showBookmarks
                    if (section === "2") return generalSettings.showDevices
                    if (section === "3") return generalSettings.showRecent
                    return true
                }
                width: visible ? sideList.width : 0
                height: visible ? (root.expanded ? 24 : 16) : 0
                color: "transparent"

                readonly property string sectionTitle: {
                    if (section === "0") return "PLACES"
                    if (section === "1") return "BOOKMARKS"
                    if (section === "2") return "DEVICES"
                    if (section === "3") return "RECENT"
                    return "OTHER"
                }

                Text {
                    visible: root.expanded
                    anchors.left: parent.left
                    anchors.leftMargin: theme ? theme.space10 : 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: parent.sectionTitle
                    color: theme ? theme.textMuted : "#8A867E"
                    font.pixelSize: theme ? theme.fontLabel : 10
                    font.bold: true
                    font.letterSpacing: theme ? theme.letterSpacingLabel : 1.0
                }

                Rectangle {
                    visible: !root.expanded
                    anchors.centerIn: parent
                    width: parent.width * 0.6
                    height: 1
                    color: theme.border
                    opacity: 0.5
                }
            }

            delegate: SidebarItem {
                visible: {
                    if (model.category === 0) return generalSettings.showPlaces
                    if (model.category === 1) return generalSettings.showBookmarks
                    if (model.category === 2) return generalSettings.showDevices
                    if (model.category === 3) return generalSettings.showRecent
                    return true
                }
                width: visible ? sideList.width : 0
                height: visible ? (root.expanded ? 36 : 40) : 0
                text: model.name
                iconName: model.icon
                theme: root.theme
                active: root.activePath === model.path
                showAction: model.category === 1
                actionIconName: "close"
                actionToolTip: "Remove bookmark"
                expanded: root.expanded
                onClicked: root.pathActivated(model.path)
                onActionClicked: root.removeBookmarkRequested(model.path)
                onRightClicked: {
                    sidebarItemMenu.targetPath = model.path
                    sidebarItemMenu.targetName = model.name
                    sidebarItemMenu.popup()
                }
            }
        }

        // Clipboard Status
        Rectangle {
            readonly property bool narrowExpanded: root.expanded && root.width < 220
            visible: !!(root.docIntelController && root.expanded)
            Layout.fillWidth: true
            Layout.preferredHeight: 84
            color: theme.surfaceMuted
            border.color: theme.border
            border.width: 1
            radius: 8
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.bottomMargin: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6
                Text {
                    text: "DOC INTEL"
                    color: theme.textMuted
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1.0
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    Button {
                        text: "Inbox"
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        onClicked: root.openDocInboxRequested()
                        background: Rectangle { radius: 6; color: hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                        contentItem: RowLayout {
                            spacing: 4
                            Icon { name: "folder-open"; iconSize: 12; color: theme.textSecondary }
                            Text {
                                text: parent.parent.text
                                color: theme.textPrimary
                                font.pixelSize: 11
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                        ToolTip.visible: hovered && parent.parent.narrowExpanded
                        ToolTip.text: "Doc Intel: Inbox"
                    }
                    Button {
                        text: "Dupes"
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        onClicked: root.openDocDuplicatesRequested()
                        background: Rectangle { radius: 6; color: hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                        contentItem: RowLayout {
                            spacing: 4
                            Icon { name: "copy"; iconSize: 12; color: theme.textSecondary }
                            Text {
                                text: parent.parent.text
                                color: theme.textPrimary
                                font.pixelSize: 11
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                        ToolTip.visible: hovered && parent.parent.narrowExpanded
                        ToolTip.text: "Doc Intel: Duplicates"
                    }
                    Button {
                        text: "Health"
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        onClicked: root.openDocHealthRequested()
                        background: Rectangle { radius: 6; color: hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                        contentItem: RowLayout {
                            spacing: 4
                            Icon { name: "chart-pie"; iconSize: 12; color: theme.textSecondary }
                            Text {
                                text: parent.parent.text
                                color: theme.textPrimary
                                font.pixelSize: 11
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                        ToolTip.visible: hovered && parent.parent.narrowExpanded
                        ToolTip.text: "Doc Intel: Health"
                    }
                }
            }
        }

        // Doc Intel (Collapsed)
        Rectangle {
            visible: !!(root.docIntelController && !root.expanded)
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: theme.surfaceMuted
            border.color: theme.border
            border.width: 1
            radius: 8
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.bottomMargin: 8

            RowLayout {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 6

                Repeater {
                    model: [
                        { label: "I", tip: "Doc Intel: Inbox", cb: () => root.openDocInboxRequested() },
                        { label: "D", tip: "Doc Intel: Duplicates", cb: () => root.openDocDuplicatesRequested() },
                        { label: "H", tip: "Doc Intel: Health", cb: () => root.openDocHealthRequested() }
                    ]
                    delegate: Button {
                        required property var modelData
                        Layout.fillWidth: true
                        text: modelData.label
                        font.pixelSize: 12
                        font.bold: true
                        onClicked: modelData.cb()
                        background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                        ToolTip.visible: hovered
                        ToolTip.text: modelData.tip
                    }
                }
            }
        }

        Rectangle {
            id: clipboardBar
            visible: !!(root.activeController && root.activeController.hasClipboard)
            Layout.fillWidth: true
            Layout.preferredHeight: root.expanded ? 64 : 50
            color: theme.surfaceMuted
            border.color: theme.border
            border.width: 1
            radius: 8
            Layout.margins: 8

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                Icon {
                    name: root.activeController && root.activeController.isCutOp ? "cut" : "copy"
                    iconSize: 16
                    color: theme.accent
                }

                Column {
                    Layout.fillWidth: true
                    visible: root.expanded
                    Text {
                        text: root.activeController && root.activeController.isCutOp ? "Cut Pending" : "Copy Pending"
                        color: theme.textSecondary
                        font.pixelSize: 10
                        font.bold: true
                    }
                    Text {
                        text: {
                            if (!root.activeController || !root.activeController.clipboardPaths || root.activeController.clipboardPaths.length === 0) return ""
                            let p = root.activeController.clipboardPaths[0]
                            return p.substring(p.lastIndexOf("/") + 1)
                        }
                        color: theme.textPrimary
                        font.pixelSize: 12
                        elide: Text.ElideMiddle
                        width: parent.width
                    }
                }

                ThemedIconButton {
                    theme: root.theme
                    iconName: "close"
                    iconSize: 14
                    toolTip: "Clear Clipboard"
                    onClicked: root.activeController.clearClipboard()
                }
            }
        }
    }

    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: theme.border
    }
}
