import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var placesModel
    property string activePath: ""
    property string homePath: ""
    property bool expanded: true
    property var activeController: null

    signal pathActivated(string path)
    signal removeBookmarkRequested(string path)
    signal toggleExpanded()

    color: theme.sidebar

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
                anchors.centerIn: parent
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
                width: sideList.width
                height: root.expanded ? 24 : 16
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
                width: sideList.width
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
            }
        }

        // Clipboard Status
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
                            if (!root.activeController || !root.activeController.clipboardPath) return ""
                            let p = root.activeController.clipboardPath
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
