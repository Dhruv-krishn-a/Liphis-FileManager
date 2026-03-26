import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var activeController
    readonly property bool compact: width < 300
    property var metadata: (activeController && activeController.hasSelection)
        ? activeController.metadataForPath(activeController.selectedPath)
        : null

    signal analyseFolderRequested(string path)
    signal toggleExpandRequested()
    property bool expanded: true

    color: theme.surface
    border.color: theme.border
    border.width: 1

    function formatSize(bytes) {
        if (!bytes) return "0 B"
        var units = ["B", "KB", "MB", "GB", "TB"]
        var i = 0
        var value = bytes
        while (value >= 1024 && i < units.length - 1) {
            value /= 1024
            i++
        }
        return value.toFixed(1) + " " + units[i]
    }

    function formatDate(secs) {
        if (!secs) return "--"
        return new Date(secs * 1000).toLocaleString(Qt.locale(), "ddd, d MMM yyyy hh:mm ap")
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: root.width - ((theme ? (root.compact ? theme.space12 : theme.space16) : 16) * 2)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: theme ? (root.compact ? theme.space12 : theme.space16) : 12
            anchors.bottomMargin: theme ? (root.compact ? theme.space12 : theme.space16) : 12
            spacing: theme ? (root.compact ? theme.space12 : theme.space16) : 12

            RowLayout {
                Layout.fillWidth: true
                spacing: theme.space8

                Text {
                    text: "Inspector"
                    color: theme.textSecondary
                    font.pixelSize: theme.fontLabel
                    font.letterSpacing: 0.8
                }

                Item { Layout.fillWidth: true }

                ToolButton {
                    text: root.expanded ? "Collapse" : "Expand"
                    font.pixelSize: theme.fontLabel
                    onClicked: root.toggleExpandRequested()
                    background: Rectangle {
                        radius: theme.rSm
                        color: parent.hovered ? theme.hover : theme.surfaceRaised
                        border.color: theme.border
                        border.width: 1
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 86
                visible: !root.metadata
                radius: theme.rMd
                color: theme.surfaceRaised
                border.color: theme.border
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: theme.space12
                    spacing: 4

                    Text {
                        text: "No Selection"
                        color: theme.textPrimary
                        font.pixelSize: theme.fontBody + 1
                        font.weight: Font.Medium
                    }
                    Text {
                        text: "Select a file or folder to see preview and details."
                        color: theme.textSecondary
                        font.pixelSize: theme.fontBody - 1
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }
            }

            PanelSection {
                visible: !!root.metadata
                theme: root.theme
                title: "PREVIEW"

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.compact
                        ? Math.min(184, root.width * 0.64)
                        : Math.min(208, root.width * 0.70)
                    radius: theme.rLg
                    color: theme.surfaceRaised
                    border.color: theme.border
                    border.width: 1
                    clip: true

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 1
                        radius: theme.rLg - 1
                        color: theme.surfaceMuted
                    }

                    Image {
                        anchors.fill: parent
                        anchors.margins: theme ? theme.space16 : 16
                        fillMode: Image.PreserveAspectFit
                        source: (root.metadata && root.metadata.thumbnail) ? root.metadata.thumbnail : ""
                        asynchronous: true

                        FileIcon {
                            anchors.centerIn: parent
                            width: 64
                            height: 64
                            isDir: !!(root.metadata && root.metadata.isDir)
                            iconName: root.metadata ? root.metadata.iconName : ""
                            theme: root.theme
                            visible: parent.status !== Image.Ready
                        }
                    }
                }
            }

            PanelSection {
                visible: !!root.metadata
                theme: root.theme
                title: "DETAILS"

                Rectangle {
                    id: detailsCard
                    Layout.fillWidth: true
                    radius: theme.rMd
                    color: theme.surfaceRaised
                    border.color: theme.border
                    border.width: 1
                    implicitHeight: detailsColumn.implicitHeight + (theme.space12 * 2)

                    ColumnLayout {
                        id: detailsColumn
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: theme.space12
                        spacing: 10

                        Text {
                                text: root.metadata ? root.metadata.name : ""
                                color: theme.textPrimary
                                font.pixelSize: root.compact ? (theme.fontTitle - 1) : theme.fontTitle
                                font.weight: Font.DemiBold
                            elide: Text.ElideMiddle
                            Layout.fillWidth: true
                        }

                        Rectangle {
                            radius: theme.rSm
                            color: theme.surfaceMuted
                            border.color: theme.border
                            border.width: 1
                            Layout.alignment: Qt.AlignLeft
                            implicitHeight: 24
                            implicitWidth: typeText.implicitWidth + 14

                            Text {
                                id: typeText
                                anchors.centerIn: parent
                                text: root.metadata ? (root.metadata.isDir ? "Folder" : root.metadata.mimeType) : ""
                                color: theme.textSecondary
                                font.pixelSize: root.compact ? (theme.fontBody - 2) : (theme.fontBody - 1)
                                elide: Text.ElideRight
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: theme.border }

                        Repeater {
                            model: [
                                { key: "Size", value: root.metadata ? (root.metadata.isDir ? "Folder" : root.formatSize(root.metadata.size)) : "" },
                                { key: "Modified", value: root.metadata ? root.formatDate(root.metadata.mtime) : "" },
                                { key: "Permissions", value: root.metadata ? root.metadata.permissions : "" },
                                { key: "Owner", value: root.metadata ? (root.metadata.owner + ":" + root.metadata.group) : "" }
                            ]

                            delegate: Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: root.compact ? 28 : 30
                                color: "transparent"

                                RowLayout {
                                    anchors.fill: parent
                                    spacing: theme.space8

                                    Text {
                                        Layout.preferredWidth: root.compact ? 72 : 82
                                        text: modelData.key
                                        color: theme.textMuted
                                        font.pixelSize: theme.fontBody - 1
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.value
                                        color: theme.textPrimary
                                        font.pixelSize: theme.fontBody
                                        font.family: modelData.key === "Permissions" ? "Monospace" : ""
                                        elide: Text.ElideRight
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Button {
                id: analyseBtn
                visible: !!(root.metadata && root.metadata.isDir)
                Layout.fillWidth: true
                implicitHeight: root.compact ? 34 : 38
                text: "Analyse Disk Usage"
                onClicked: if (root.metadata) root.analyseFolderRequested(root.metadata.path)
                background: Rectangle {
                    radius: theme ? theme.rSm : 8
                    color: analyseBtn.hovered ? theme.hover : theme.surfaceRaised
                    border.color: theme.border
                    border.width: 1
                }
                contentItem: RowLayout {
                    anchors.centerIn: parent
                    spacing: theme ? theme.space8 : 8
                    Icon { name: "info"; size: 14; color: theme.accent }
                    Text {
                        text: analyseBtn.text
                        color: theme.textPrimary
                        font.pixelSize: theme.fontBody
                        font.weight: Font.Medium
                    }
                }
            }
        }
    }
}
