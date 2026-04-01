import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import liphis

Rectangle {
    id: root

    property var theme
    property var activeController
    property var docIntelController
    readonly property bool compact: width < 300
    
    readonly property bool hasSelection: !!(activeController
                                            && activeController.hasSelection !== undefined
                                            && activeController.hasSelection
                                            && activeController.selectionRevision !== undefined
                                            && activeController.selectionRevision >= 0)
    
    property var metadata: null

    property string gitOutput: ""
    property string activeGitCmd: ""
    property var gitLines: []
    property var timelineData: null
    readonly property int panelInset: theme ? (root.compact ? theme.space8 : theme.space12) : (root.compact ? 8 : 12)
    readonly property int bodyFont: root.compact ? 10 : 11
    readonly property int titleFont: root.compact ? 12 : 13
    readonly property int chipHeight: root.compact ? 24 : 28
    readonly property int actionButtonHeight: root.compact ? 30 : 34

    component InspectorActionButton: Button {
        id: actionButton
        implicitHeight: root.actionButtonHeight
        font.pixelSize: root.bodyFont
        property string helpText: ""
        hoverEnabled: true
        contentItem: Text {
            text: actionButton.text
            color: actionButton.enabled ? theme.textPrimary : theme.textMuted
            font.pixelSize: actionButton.font.pixelSize
            font.weight: Font.Medium
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        ToolTip.visible: hovered && helpText.length > 0
        ToolTip.text: helpText
        background: Rectangle {
            radius: 6
            color: !actionButton.enabled
                   ? theme.surface
                   : (parent.hovered ? theme.hover : theme.surfaceMuted)
            border.color: theme.border
        }
    }

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
        if (!activeController || activeController.runGitCommand === undefined) return
        activeGitCmd = "git " + cmd
        gitOutput = activeController.runGitCommand(cmd, root.metadata ? root.metadata.path : "")
        gitLines = formatGitLines(gitOutput)
    }

    function formatGitLines(text) {
        if (!text || text.length === 0) return []
        return text.split("\n")
    }

    function gitLineColor(line) {
        if (!line || line.length === 0) return theme.textMuted
        const t = line.trim()
        if (t.startsWith("diff --git")) return "#4F8CC9"
        if (t.startsWith("index ")) return theme.textMuted
        if (t.startsWith("@@")) return "#A57CC5"
        if (t.startsWith("+++ ") || t.startsWith("--- ")) return "#5B9CC9"
        if (line.startsWith("+") && !t.startsWith("+++")) return "#4FA86D"
        if (line.startsWith("-") && !t.startsWith("---")) return "#CB6B6B"
        if (t.startsWith("fatal:") || t.startsWith("error:")) return theme.error
        if (t.startsWith("modified:") || t.startsWith("changes not staged")) return "#D99A5D"
        if (t.startsWith("deleted:")) return "#D46A6A"
        if (t.startsWith("new file:") || t.startsWith("created:")) return "#67B87B"
        if (t.startsWith("renamed:")) return "#5BA8D8"
        if (t.startsWith("Untracked files:")) return "#E0A86A"
        if (t.startsWith("On branch") || t.startsWith("HEAD")) return theme.accent
        if (t.startsWith("nothing to commit") || t.startsWith("working tree clean")) return "#67B87B"
        return theme.textPrimary
    }

    function gitLineBackground(line) {
        if (!line || line.length === 0) return "transparent"
        const t = line.trim()
        if (t.startsWith("diff --git") || t.startsWith("@@")) {
            return theme.isDark ? Qt.rgba(1, 1, 1, 0.04) : Qt.rgba(0, 0, 0, 0.03)
        }
        if (line.startsWith("+") && !t.startsWith("+++")) {
            return theme.isDark ? Qt.rgba(0.31, 0.66, 0.43, 0.15) : Qt.rgba(0.31, 0.66, 0.43, 0.10)
        }
        if (line.startsWith("-") && !t.startsWith("---")) {
            return theme.isDark ? Qt.rgba(0.80, 0.42, 0.42, 0.16) : Qt.rgba(0.80, 0.42, 0.42, 0.10)
        }
        return "transparent"
    }

    function gitLineBold(line) {
        if (!line || line.length === 0) return false
        const t = line.trim()
        return t.startsWith("diff --git")
            || t.startsWith("@@")
            || t.startsWith("On branch")
            || t.startsWith("Untracked files:")
            || t.startsWith("Changes to be committed:")
            || t.startsWith("Changes not staged for commit:")
            || t.startsWith("nothing to commit")
    }

    function refreshMetadata() {
        if (!activeController || activeController.metadataForPath === undefined || activeController.getFolderMetadata === undefined) {
            metadata = null
            timelineData = null
            return
        }

        if (hasSelection && activeController.selectedPath && activeController.selectedPath.length > 0) {
            metadata = activeController.metadataForPath(activeController.selectedPath)
        } else {
            metadata = activeController.getFolderMetadata(activeController.currentPath)
        }
        refreshTimeline()
    }

    function refreshTimeline() {
        if (!docIntelController || !metadata || !metadata.path) {
            timelineData = null
            return
        }
        timelineData = docIntelController.timelineForPath(metadata.path)
    }

    function detailsRows() {
        if (!root.metadata) return []
        const m = root.metadata
        const rows = []
        const itemsCount = (m.fileCount !== undefined && m.folderCount !== undefined)
            ? (m.fileCount + m.folderCount)
            : -1

        const sizeValue = (m.isDir && m.sizeComputed === false)
            ? (root.formatSize(m.size || 0) + " (quick)")
            : root.formatSize(m.size || 0)
        rows.push({ key: "Size", value: sizeValue })
        if (m.isDir && itemsCount >= 0) rows.push({ key: "Items", value: itemsCount + " (" + (m.folderCount || 0) + " folders, " + (m.fileCount || 0) + " files)" })
        if (m.mtime) rows.push({ key: "Modified", value: root.formatDate(m.mtime) })
        if (m.birthTime) rows.push({ key: "Created", value: root.formatDate(m.birthTime) })
        else if (m.ctime) rows.push({ key: "Created", value: root.formatDate(m.ctime) })
        if (m.atime) rows.push({ key: "Accessed", value: root.formatDate(m.atime) })
        if (m.owner) rows.push({ key: "Owner", value: m.owner })
        if (m.group) rows.push({ key: "Group", value: m.group })
        if (m.permissions) rows.push({ key: "Permissions", value: m.permissions })
        if (m.mimeType) rows.push({ key: "MIME", value: m.mimeType })
        if (!m.isDir && m.suffix) rows.push({ key: "Extension", value: "." + m.suffix })
        if (m.path) rows.push({ key: "Path", value: m.path })

        return rows
    }

    Component.onCompleted: refreshMetadata()
    onActiveControllerChanged: refreshMetadata()
    onHasSelectionChanged: refreshMetadata()

    Connections {
        target: (activeController && activeController.metadataForPath !== undefined) ? activeController : null
        function onCurrentPathChanged() { root.refreshMetadata() }
        function onSelectedPathChanged() { root.refreshMetadata() }
        function onSelectedPathsChanged() { root.refreshMetadata() }
        function onFolderSizeResolved(path, size) {
            if (!root.metadata || !root.metadata.path) return
            if (root.metadata.path !== path) return
            root.metadata.size = size
            root.metadata.sizeComputed = true
        }
    }

    Connections {
        target: docIntelController
        enabled: !!docIntelController
        function onGroupsChanged() { root.refreshTimeline() }
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: root.width - ((theme ? (root.compact ? theme.space8 : theme.space16) : (root.compact ? 8 : 16)) * 2)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: theme ? (root.compact ? theme.space8 : theme.space16) : (root.compact ? 8 : 16)
            anchors.bottomMargin: theme ? (root.compact ? theme.space8 : theme.space16) : (root.compact ? 8 : 16)
            spacing: theme ? (root.compact ? theme.space8 : theme.space16) : (root.compact ? 8 : 16)

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 48
                radius: theme.rSm
                color: root.hasSelection ? theme.selection : theme.surfaceMuted
                
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: root.panelInset; anchors.rightMargin: root.panelInset
                    spacing: root.compact ? 8 : 10
                    Icon {
                        name: root.hasSelection ? "mouse-pointer" : "folder-open"
                        iconSize: root.compact ? 14 : 16
                        color: theme.accent
                    }
                    Text {
                        text: root.hasSelection ? "Selection Info" : "Folder Insights"
                        color: theme.textPrimary
                        font.pixelSize: root.titleFont; font.bold: true
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
                        ? Math.min(168, root.width * 0.60)
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
                        anchors.margins: root.compact ? root.panelInset : (theme ? theme.space16 : 16)
                        fillMode: Image.PreserveAspectFit
                        source: (root.metadata && root.metadata.thumbnail !== undefined) ? root.metadata.thumbnail : ""
                        asynchronous: true

                        FileIcon {
                            anchors.centerIn: parent
                            width: root.compact ? 56 : 64
                            height: root.compact ? 56 : 64
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
                        anchors.margins: root.panelInset
                        spacing: root.compact ? 8 : 10

                        Text {
                            text: (root.metadata && root.metadata.name !== undefined) ? root.metadata.name : "No Selection"
                            color: theme.textPrimary
                            font.pixelSize: root.compact ? (theme.fontTitle - 2) : theme.fontTitle
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
                                font.pixelSize: root.bodyFont
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        InspectorActionButton {
                            visible: !!(root.metadata && root.metadata.isDir && root.metadata.path && root.metadata.sizeComputed === false)
                            text: "Compute Full Size"
                            helpText: "Runs recursive size calculation for this folder."
                            Layout.alignment: Qt.AlignLeft
                            Layout.preferredWidth: root.compact ? 150 : 170
                            onClicked: if (activeController && root.metadata) activeController.requestFolderSize(root.metadata.path)
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: theme.border }

                        Repeater {
                            model: root.detailsRows()

                            delegate: Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: 26
                                color: "transparent"

                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 8

                                    Text {
                                        Layout.preferredWidth: root.compact ? 58 : 70
                                        text: modelData.key
                                        color: theme.textMuted
                                        font.pixelSize: root.bodyFont
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.value || ""
                                        color: theme.textPrimary
                                        font.pixelSize: root.bodyFont
                                        elide: Text.ElideMiddle
                                        wrapMode: root.compact ? Text.WrapAtWordBoundaryOrAnywhere : Text.NoWrap
                                        maximumLineCount: root.compact ? 2 : 1
                                    }
                                }
                            }
                        }
                    }
                }
            }

            PanelSection {
                theme: root.theme
                title: "DOCUMENT LIFECYCLE"
                visible: !!(root.metadata && root.metadata.path && docIntelController)

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Text {
                            Layout.fillWidth: true
                            text: {
                                const tl = root.timelineData
                                return tl && tl.versionCount ? "Version Stack: v" + tl.versionCount : "No version stack detected"
                            }
                            color: theme.textSecondary
                            font.pixelSize: root.bodyFont
                            elide: Text.ElideRight
                        }
                        InspectorActionButton {
                            text: (docIntelController && docIntelController.busy) ? "Scanning..." : "Scan Folder"
                            helpText: "Index documents in this folder so lifecycle actions can work."
                            visible: !!activeController
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: root.compact ? Math.min(150, parent.width) : 120
                            enabled: !!(docIntelController && !docIntelController.busy)
                            onClicked: if (docIntelController && activeController) docIntelController.scanFolder(activeController.currentPath)
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: root.compact ? 2 : 3
                        columnSpacing: root.compact ? 4 : 6
                        rowSpacing: root.compact ? 4 : 6
                        InspectorActionButton {
                            text: "Mark Current"
                            helpText: "Marks selected document as current version and archives previous current."
                            Layout.fillWidth: true
                            enabled: !!(root.metadata && !root.metadata.isDir)
                            onClicked: if (docIntelController && root.metadata) docIntelController.setDocumentStatus(root.metadata.path, "Current")
                        }
                        InspectorActionButton {
                            text: "Mark Final"
                            helpText: "Marks selected document as final. File is not moved or modified."
                            Layout.fillWidth: true
                            enabled: !!(root.metadata && !root.metadata.isDir)
                            onClicked: if (docIntelController && root.metadata) docIntelController.setDocumentStatus(root.metadata.path, "Final")
                        }
                        InspectorActionButton {
                            text: "Archive"
                            helpText: "Archives selected document in lifecycle metadata only. Does not delete the file."
                            Layout.fillWidth: true
                            Layout.columnSpan: root.compact ? 2 : 1
                            enabled: !!(root.metadata && !root.metadata.isDir)
                            onClicked: if (docIntelController && root.metadata) docIntelController.setDocumentStatus(root.metadata.path, "Archived")
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: theme.border
                    }

                    Repeater {
                        model: {
                            const tl = root.timelineData
                            if (!tl || !tl.versions) return []
                            return tl.versions
                        }

                        delegate: Rectangle {
                            required property var modelData
                            Layout.fillWidth: true
                            color: modelData.path === (root.timelineData ? root.timelineData.currentPath : "") ? theme.selection : "transparent"
                            radius: 6
                            implicitHeight: 28

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.name + "  [" + (modelData.status || "Draft") + "]"
                                    color: theme.textPrimary
                                    font.pixelSize: 10
                                    elide: Text.ElideMiddle
                                }
                                InspectorActionButton {
                                    id: setButton
                                    text: "Set"
                                    helpText: "Set this file as current version for this document group."
                                    visible: !!(root.timelineData && root.timelineData.groupId)
                                    implicitHeight: root.compact ? 24 : 26
                                    implicitWidth: root.compact ? 68 : 72
                                    font.pixelSize: root.compact ? 9 : 10
                                    onClicked: {
                                        const tl = root.timelineData
                                        if (docIntelController && tl) docIntelController.setCurrentVersion(tl.groupId, modelData.path)
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
                                implicitHeight: root.chipHeight
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.text: "Run: git " + modelData.cmd
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
                        implicitHeight: gitColumn.implicitHeight + 6
                        color: "transparent"

                        ColumnLayout {
                            id: gitColumn
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 6

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: root.activeGitCmd
                                    color: theme.textMuted
                                    font.pixelSize: root.compact ? 9 : 10
                                    font.family: "Monospace"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                                ToolButton {
                                    text: "✕"
                                    font.pixelSize: 11
                                    implicitWidth: 22
                                    implicitHeight: 22
                                    onClicked: {
                                        root.gitOutput = ""
                                        root.gitLines = []
                                    }
                                    background: Rectangle {
                                        radius: 4
                                        color: parent.hovered ? theme.hover : "transparent"
                                    }
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: theme.border
                            }

                            ListView {
                                id: gitListView
                                Layout.fillWidth: true
                                Layout.preferredHeight: Math.min(root.compact ? 220 : 260, Math.max(root.compact ? 84 : 96, gitLines.length * (root.compact ? 18 : 20)))
                                clip: true
                                model: root.gitLines
                                spacing: 2
                                delegate: Rectangle {
                                    required property string modelData
                                    width: ListView.view ? ListView.view.width : 0
                                    color: root.gitLineBackground(modelData)
                                    radius: color === "transparent" ? 0 : 3
                                    implicitHeight: gitLineText.implicitHeight + 4

                                    Text {
                                        id: gitLineText
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.leftMargin: 6
                                        anchors.rightMargin: 6
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: modelData
                                        color: root.gitLineColor(modelData)
                                        font.family: "Monospace"
                                        font.pixelSize: root.compact ? 10 : 11
                                        font.bold: root.gitLineBold(modelData)
                                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                        textFormat: Text.PlainText
                                    }
                                }
                                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                            }
                        }
                    }
                }
            }

            Button {
                id: analyseBtn
                visible: !!(root.metadata && root.metadata.isDir)
                Layout.fillWidth: true
                implicitHeight: root.compact ? 38 : 42
                text: "Analyse Disk Usage"
                hoverEnabled: true
                ToolTip.visible: hovered
                ToolTip.text: "Analyze folder size distribution and largest files."
                onClicked: if (root.metadata) root.analyseFolderRequested(root.metadata.path)
                background: Rectangle {
                    radius: theme.rMd
                    color: analyseBtn.hovered ? theme.accentSoft : theme.surfaceRaised
                    border.color: analyseBtn.hovered ? theme.accent : theme.border
                    border.width: 1
                }
                contentItem: RowLayout {
                    anchors.centerIn: parent
                    spacing: 10
                    Icon { name: "chart-pie"; iconSize: 14; color: theme.accent }
                    Text {
                        text: analyseBtn.text
                        color: theme.textPrimary
                        font.pixelSize: root.compact ? (theme.fontBody - 1) : theme.fontBody
                        font.weight: Font.Medium
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: theme ? theme.space12 : 12
            }
        }
    }
}
