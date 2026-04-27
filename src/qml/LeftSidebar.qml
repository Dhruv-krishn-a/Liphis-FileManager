import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtCore

Item {
    id: root
    property var theme: null
    property var activeController: null
    property var placesModel: null
    property var docIntelController: null
    property string activePath: ""
    property string homePath: ""
    property bool expanded: generalSettings.sidebarExpanded
    property bool narrowExpanded: !expanded && width > 100
    
    signal toggleExpanded()
    signal pathActivated(string path)
    signal removeBookmarkRequested(string path)
    signal openDocInboxRequested()
    signal openDocDuplicatesRequested()
    signal openDocHealthRequested()

    width: expanded ? (theme ? theme.sidebarWidth : 220) : (theme ? theme.sidebarWidthCollapsed : 64)
    Behavior on width { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

    Rectangle {
        anchors.fill: parent
        color: theme.surfaceMuted
    }

    Rectangle {
        anchors.right: parent.right
        width: 1; height: parent.height
        color: theme.border
        opacity: 0.5
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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // Sidebar Header / Toggle
        Rectangle {
            Layout.fillWidth: true
            height: theme.headerHeight
            color: "transparent"

            ThemedIconButton {
                id: toggleBtn
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

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            
            ColumnLayout {
                width: parent.width
                spacing: 12
                Layout.topMargin: 12
                Layout.bottomMargin: 16

                // Section: Places
                ColumnLayout {
                    visible: generalSettings.showPlaces
                    Layout.fillWidth: true
                    spacing: 2
                    
                    Text {
                        text: "PLACES"
                        Layout.leftMargin: root.expanded ? 16 : 0
                        Layout.alignment: root.expanded ? Qt.AlignLeft : Qt.AlignHCenter
                        font.pixelSize: 10
                        color: theme.textTertiary
                        visible: root.expanded || root.narrowExpanded
                        font.bold: true
                        font.letterSpacing: 1.0
                    }
                    
                    SidebarItem { theme: root.theme; active: root.activePath === root.homePath; iconName: "home"; text: "Home"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated(root.homePath) }
                    SidebarItem { theme: root.theme; active: root.activePath.endsWith("/Documents"); iconName: "folder"; text: "Documents"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated(root.homePath + "/Documents") }
                    SidebarItem { theme: root.theme; active: root.activePath.endsWith("/Downloads"); iconName: "download"; text: "Downloads"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated(root.homePath + "/Downloads") }
                    SidebarItem { theme: root.theme; active: root.activePath.endsWith("/Pictures"); iconName: "photo"; text: "Pictures"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated(root.homePath + "/Pictures") }
                    SidebarItem { theme: root.theme; active: root.activePath.endsWith("/Videos"); iconName: "video"; text: "Videos"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated(root.homePath + "/Videos") }
                    SidebarItem { theme: root.theme; active: root.activePath.endsWith("/Music"); iconName: "music"; text: "Music"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated(root.homePath + "/Music") }
                    SidebarItem { theme: root.theme; active: root.activePath === "trash:///"; iconName: "trash"; text: "Trash"; expanded: root.expanded || root.narrowExpanded; onClicked: root.pathActivated("trash:///") }
                }

                // Section: Devices
                ColumnLayout {
                    visible: generalSettings.showDevices && root.placesModel && root.placesModel.rowCount() > 0
                    Layout.fillWidth: true
                    Layout.topMargin: 8
                    spacing: 2

                    Text {
                        text: "DEVICES"
                        Layout.leftMargin: root.expanded ? 16 : 0
                        Layout.alignment: root.expanded ? Qt.AlignLeft : Qt.AlignHCenter
                        font.pixelSize: 10
                        color: theme.textTertiary
                        visible: root.expanded || root.narrowExpanded
                        font.bold: true
                        font.letterSpacing: 1.0
                    }

                    Repeater {
                        model: root.placesModel
                        SidebarItem {
                            theme: root.theme
                            active: root.activePath === model.path
                            iconName: model.icon || "drive-harddisk"
                            text: model.name
                            expanded: root.expanded || root.narrowExpanded
                            onClicked: root.pathActivated(model.path)
                        }
                    }
                }

                // Section: Recent
                ColumnLayout {
                    visible: generalSettings.showRecent && root.activeController
                    Layout.fillWidth: true
                    Layout.topMargin: 8
                    spacing: 2
                    
                    Text {
                        text: "RECENT"
                        Layout.leftMargin: root.expanded ? 16 : 0
                        Layout.alignment: root.expanded ? Qt.AlignLeft : Qt.AlignHCenter
                        font.pixelSize: 10
                        color: theme.textTertiary
                        visible: root.expanded || root.narrowExpanded
                        font.bold: true
                        font.letterSpacing: 1.0
                    }
                    
                    Repeater {
                        model: root.activeController ? root.activeController.recentPaths : []
                        SidebarItem {
                            theme: root.theme
                            active: false
                            iconName: "folder"
                            text: modelData.split('/').pop()
                            expanded: root.expanded || root.narrowExpanded
                            onClicked: root.pathActivated(modelData)
                            ToolTip.visible: hovered
                            ToolTip.text: modelData
                        }
                    }
                }
                
                Item { Layout.fillHeight: true }
            }
        }

        // Section: Doc Intel
        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: 12
            spacing: 8
            visible: root.expanded || root.narrowExpanded

            Rectangle {
                Layout.fillWidth: true; height: 1; color: theme.border; opacity: 0.2
            }

            Text {
                text: "DOC INTEL"
                font.pixelSize: 10
                color: theme.textTertiary
                font.bold: true
                font.letterSpacing: 1.0
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                Button {
                    id: inboxBtn
                    text: "Inbox"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    onClicked: root.openDocInboxRequested()
                    background: Rectangle { radius: 6; color: inboxBtn.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
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
                    id: dupesBtn
                    text: "Dupes"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    onClicked: root.openDocDuplicatesRequested()
                    background: Rectangle { radius: 6; color: dupesBtn.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
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
                    id: healthBtn
                    text: "Health"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    onClicked: root.openDocHealthRequested()
                    background: Rectangle { radius: 6; color: healthBtn.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
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
}
