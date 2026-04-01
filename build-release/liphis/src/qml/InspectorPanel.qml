import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import liphis

Rectangle {
    id: root

    property var theme
    property var activeController
    readonly property bool compact: width < 300
    
    readonly property bool hasSelection: activeController && activeController.hasSelection && (activeController.selectionRevision >= 0)
    
    // Fallback to current folder metadata if no selection
    property var metadata: hasSelection
        ? activeController.metadataForPath(activeController.selectedPath)
        : (activeController ? activeController.getFolderMetadata("") : null)

    property string gitOutput: ""
    property string activeGitCmd: ""

    signal analyseFolderRequested(string path)

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

    function runGit(cmd) {
        activeGitCmd = "git " + cmd
        gitOutput = activeController.runGitCommand(cmd, root.metadata ? root.metadata.path : "")
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

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 48
                radius: theme.rSm
                color: root.hasSelection ? theme.selection : theme.surfaceMuted
                
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                    spacing: 10
                    Icon {
                        name: root.hasSelection ? "mouse-pointer" : "folder-open"
                        iconSize: 16
                        color: theme.accent
                    }
                    Text {
                        text: root.hasSelection ? "Selection Info" : "Folder Insights"
                        color: theme.textPrimary
                        font.pixelSize: 13; font.bold: true
                        Layout.fillWidth: true
                    }
                }
            }

            PanelSection {
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
                        source: (root.metadata && root.metadata.thumbnail !== undefined) ? root.metadata.thumbnail : ""
                        asynchronous: true

                        FileIcon {
                            anchors.centerIn: parent
                            width: 64
                            height: 64
                            isDir: !!(root.metadata && root.metadata.isDir)
                            iconName: (root.metadata && root.metadata.iconName !== undefined) ? root.metadata.iconName : ""
                            theme: root.theme
                            visible: parent.status !== Image.Ready
                        }
                    }
                }
            }

            PanelSection {
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
                            text: (root.metadata && root.metadata.name !== undefined) ? root.metadata.name : "No Selection"
                            color: theme.textPrimary
                            font.pixelSize: root.compact ? (theme.fontTitle - 1) : theme.fontTitle
                            font.weight: Font.DemiBold
                            elide: Text.ElideMiddle
                            Layout.fillWidth: true
                        }

                        RowLayout {
                            spacing: 6
                            Rectangle {
                                radius: theme.rSm
                                color: theme.surfaceMuted
                                border.color: theme.border
                                border.width: 1
                                implicitHeight: 22
                                implicitWidth: typeText.implicitWidth + 12
                                Text {
                                    id: typeText
                                    anchors.centerIn: parent
                                    text: root.metadata ? (root.metadata.isDir ? "Folder" : (root.metadata.mimeType || "Unknown")) : ""
                                    color: theme.textSecondary
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                }
                            }
                            Text {
                                visible: !!(root.metadata && root.metadata.fileCount !== undefined)
                                text: (root.metadata && root.metadata.fileCount !== undefined) ? qsTr("%1 items").arg(root.metadata.fileCount + root.metadata.folderCount) : ""
                                color: theme.textMuted
                                font.pixelSize: 11
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: theme.border }

                        Repeater {
                            model: [
                                { key: "Size", value: (root.metadata && root.metadata.size !== undefined) ? root.formatSize(root.metadata.size) : "Calculating..." },
                                { key: "Modified", value: (root.metadata && root.metadata.mtime) ? root.formatDate(root.metadata.mtime) : "" },
                                { key: "Path", value: (root.metadata && root.metadata.path) ? root.metadata.path : "" }
                            ]

                            delegate: Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: 26
                                color: "transparent"

                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 8

                                    Text {
                                        Layout.preferredWidth: 70
                                        text: modelData.key
                                        color: theme.textMuted
                                        font.pixelSize: 11
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.value || ""
                                        color: theme.textPrimary
                                        font.pixelSize: 11
                                        elide: Text.ElideLeft
                                    }
                                }
                            }
                        }
                    }
                }
            }

            PanelSection {
                theme: root.theme
                title: "GIT INTELLIGENCE"
                visible: root.activeController && root.activeController.gitStatus !== undefined

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Flow {
                        Layout.fillWidth: true
                        spacing: 6
                        
                        Repeater {
                            model: [
                                { label: "Status", cmd: "status" },
                                { label: "Diff", cmd: "diff" },
                                { label: "Log", cmd: "log --oneline -n 20" },
                                { label: "Branch", cmd: "branch" },
                                { label: "Remote", cmd: "remote -v" }
                            ]
                            delegate: Button {
                                text: modelData.label
                                flat: true
                                font.pixelSize: 10
                                font.bold: true
                                implicitHeight: 28
                                onClicked: root.runGit(modelData.cmd)
                                background: Rectangle {
                                    radius: 6
                                    color: parent.hovered ? theme.hover : theme.surfaceMuted
                                    border.color: theme.border
                                }
                                contentItem: Text {
                                    text: parent.text; font: parent.font
                                    color: theme.accent; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }

                    Rectangle {
                        visible: root.gitOutput !== ""
                        Layout.fillWidth: true
                        Layout.preferredHeight: Math.min(300, gitText.implicitHeight + 20)
                        color: theme.isDark ? "#121212" : "#F8F8F8"
                        radius: 8
                        border.color: theme.border
                        clip: true

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 4
                            
                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: root.activeGitCmd
                                    color: theme.textMuted
                                    font.pixelSize: 9
                                    font.family: "Monospace"
                                    Layout.fillWidth: true
                                }
                                ToolButton {
                                    text: "✕"
                                    font.pixelSize: 10
                                    implicitWidth: 20; implicitHeight: 20
                                    onClicked: root.gitOutput = ""
                                    background: null
                                }
                            }

                            ScrollView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                TextArea {
                                    id: gitText
                                    text: root.gitOutput
                                    readOnly: true
                                    font.family: "Monospace"
                                    font.pixelSize: 11
                                    color: theme.textPrimary
                                    background: null
                                    selectByMouse: true
                                    wrapMode: Text.NoWrap
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
                implicitHeight: 38
                text: "Analyse Disk Usage"
                onClicked: if (root.metadata) root.analyseFolderRequested(root.metadata.path)
                background: Rectangle {
                    radius: theme.rSm
                    color: analyseBtn.hovered ? theme.hover : theme.surfaceRaised
                    border.color: theme.border
                    border.width: 1
                }
                contentItem: RowLayout {
                    anchors.centerIn: parent
                    spacing: 8
                    Icon { name: "info"; iconSize: 14; color: theme.accent }
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
