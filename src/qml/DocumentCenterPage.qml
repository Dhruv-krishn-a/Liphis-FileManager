import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property var theme
    property var controller
    property var toastManager
    property string section: "inbox"
    property int maxExactToShow: 120
    property int maxNearToShow: 320
    property int maxPathsPerExactSet: 6
    property bool autoIndexedOnce: false

    function sectionIndex(s) {
        if (s === "dupes") return 1
        if (s === "health") return 2
        return 0
    }

    function formatSize(bytes) {
        var b = Number(bytes || 0)
        if (b < 1024) return b + " B"
        if (b < 1024 * 1024) return (b / 1024).toFixed(1) + " KB"
        if (b < 1024 * 1024 * 1024) return (b / (1024 * 1024)).toFixed(1) + " MB"
        return (b / (1024 * 1024 * 1024)).toFixed(2) + " GB"
    }

    function exactModel() {
        if (!controller || !controller.duplicateExact) return []
        var src = controller.duplicateExact
        return src.length > maxExactToShow ? src.slice(0, maxExactToShow) : src
    }

    function nearModel() {
        if (!controller || !controller.duplicateNear) return []
        var src = controller.duplicateNear
        return src.length > maxNearToShow ? src.slice(0, maxNearToShow) : src
    }

    function inboxMatchedModel() {
        if (!controller || !controller.inboxItems) return []
        return controller.inboxItems.filter(function (x) { return !!(x.matchedGroup && x.matchedGroup.length > 0) })
    }

    function inboxUnmatchedModel() {
        if (!controller || !controller.inboxItems) return []
        return controller.inboxItems.filter(function (x) { return !(x.matchedGroup && x.matchedGroup.length > 0) })
    }

    function metric(key, fallback) {
        if (!controller || !controller.healthReport) return fallback
        var v = controller.healthReport[key]
        if (v === undefined || v === null) return fallback
        return v
    }

    function pct(part, total) {
        var t = Number(total || 0)
        if (t <= 0) return 0
        return Math.max(0, Math.min(1, Number(part || 0) / t))
    }

    function healthLabel(score) {
        if (score >= 85) return "Healthy"
        if (score >= 60) return "Needs Attention"
        return "At Risk"
    }

    function inboxReason(item) {
        if (!item) return ""
        if (item.suggestion === "replace") return "Looks like an updated version. You can replace the current file in that version stack."
        if (item.suggestion === "version") return "Looks like another revision of an existing doc. Add it to the same version stack."
        return "No matching version stack found. Add as a new tracked document or ignore."
    }

    function ensureCurrentFolderIndexed() {
        if (autoIndexedOnce) return
        if (!controller || !appWindow || !appWindow.activeController) return
        if (controller.healthReport && Number(controller.healthReport.totalDocs || 0) > 0) return
        autoIndexedOnce = true
        controller.scanFolder(appWindow.activeController.currentPath)
    }

    Component.onCompleted: ensureCurrentFolderIndexed()

    component PrimaryActionButton: Button {
        id: actionBtn
        property color tint: theme.accent
        implicitHeight: 34
        implicitWidth: Math.max(92, contentItem.implicitWidth + 24)
        leftPadding: 10
        rightPadding: 10
        topPadding: 0
        bottomPadding: 0
        font.pixelSize: 12
        font.weight: Font.DemiBold

        background: Rectangle {
            radius: 8
            border.width: actionBtn.down ? 2 : 1
            border.color: !actionBtn.enabled
                ? theme.border
                : (actionBtn.down ? Qt.darker(actionBtn.tint, 1.25)
                                  : (actionBtn.hovered ? actionBtn.tint : theme.border))
            color: !actionBtn.enabled
                ? theme.surfaceMuted
                : (actionBtn.down ? theme.selection
                                  : (actionBtn.hovered ? theme.selection : theme.surfaceRaised))
            Behavior on color { ColorAnimation { duration: 90 } }
            Behavior on border.color { ColorAnimation { duration: 90 } }
        }

        contentItem: Text {
            text: actionBtn.text
            color: actionBtn.enabled ? theme.textPrimary : theme.textMuted
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font: actionBtn.font
            elide: Text.ElideRight
        }
    }

    component StatCard: Rectangle {
        id: card
        property string title: ""
        property string value: ""
        property string hint: ""
        property color accentColor: theme.accent

        radius: theme.rMd
        color: theme.surfaceRaised
        border.color: theme.border
        implicitHeight: 84

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 4
            radius: 4
            color: card.accentColor
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            anchors.leftMargin: 14
            spacing: 2
            Text {
                text: card.title
                color: theme.textSecondary
                font.pixelSize: 11
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 1.0
            }
            Text {
                text: card.value
                color: theme.textPrimary
                font.pixelSize: 35
                font.weight: Font.Bold
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            Text {
                visible: card.hint.length > 0
                text: card.hint
                color: theme.textMuted
                font.pixelSize: 10
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }
    }

    component MeterRow: Item {
        id: meter
        property string label: ""
        property string valueText: ""
        property real ratio: 0
        property color fill: theme.accent
        implicitHeight: 48

        ColumnLayout {
            anchors.fill: parent
            spacing: 4
            RowLayout {
                Layout.fillWidth: true
                Text { text: meter.label; color: theme.textSecondary; font.pixelSize: 11; Layout.fillWidth: true }
                Text { text: meter.valueText; color: theme.textPrimary; font.pixelSize: 11; font.bold: true }
            }
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 8
                radius: 4
                color: theme.surfaceMuted
                border.color: theme.border
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.width * Math.max(0, Math.min(1, meter.ratio))
                    radius: 4
                    color: meter.fill
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bg
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme.space16
        spacing: theme.space12

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 88
            radius: theme.rLg
            border.color: theme.border
            color: theme.surfaceRaised

            RowLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 14

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 3
                    Text {
                        text: "Document Center"
                        color: theme.textPrimary
                        font.pixelSize: 42
                        font.weight: Font.Bold
                    }
                    Text {
                        text: "Indexed: " + root.metric("totalDocs", 0)
                              + "  |  Groups: " + root.metric("groups", 0)
                              + "  |  Exact: " + (controller ? controller.duplicateExact.length : 0)
                              + "  |  Near: " + (controller ? controller.duplicateNear.length : 0)
                        color: theme.textSecondary
                        font.pixelSize: 12
                    }
                }

                RowLayout {
                    spacing: 8
                    BusyIndicator {
                        running: !!(controller && controller.busy)
                        visible: running
                        implicitWidth: 22
                        implicitHeight: 22
                    }
                    PrimaryActionButton {
                        text: (controller && controller.busy) ? "Scanning..." : "Scan Current Folder"
                        enabled: !!(controller && !controller.busy)
                        onClicked: {
                            if (controller && appWindow && appWindow.activeController) {
                                controller.scanFolder(appWindow.activeController.currentPath)
                            }
                        }
                    }
                    PrimaryActionButton {
                        text: "Refresh Inbox"
                        onClicked: if (controller) controller.scanDownloadsInbox()
                    }
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: width > 1200 ? 4 : 2
            rowSpacing: 10
            columnSpacing: 10

            StatCard {
                Layout.fillWidth: true
                title: "Health"
                value: root.metric("score", 0) + "/100"
                hint: root.healthLabel(root.metric("score", 0))
                accentColor: root.metric("score", 0) >= 70 ? theme.success : theme.warning
            }
            StatCard {
                Layout.fillWidth: true
                title: "Documents"
                value: String(root.metric("totalDocs", 0))
                hint: "Indexed documents"
                accentColor: theme.accent
            }
            StatCard {
                Layout.fillWidth: true
                title: "Duplicate Burden"
                value: String(root.metric("exactDuplicates", 0) + root.metric("nearDuplicates", 0))
                hint: root.metric("exactDuplicates", 0) + " exact + " + root.metric("nearDuplicates", 0) + " near"
                accentColor: theme.warning
            }
            StatCard {
                Layout.fillWidth: true
                title: "Inbox Pending"
                value: String(root.metric("inbox", 0))
                hint: "Downloads waiting triage"
                accentColor: theme.textSecondary
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 40
            radius: theme.rMd
            color: theme.surfaceMuted
            border.color: theme.border

            TabBar {
                id: topTabs
                anchors.fill: parent
                anchors.margins: 2
                spacing: 6
                currentIndex: root.sectionIndex(root.section)
                onCurrentIndexChanged: {
                    if (currentIndex === 0) root.section = "inbox"
                    else if (currentIndex === 1) root.section = "dupes"
                    else root.section = "health"
                }
                background: Rectangle { color: "transparent" }

                TabButton { text: "Inbox"; font.pixelSize: 12; background: Rectangle { radius: 8; color: parent.checked ? theme.selection : "transparent" } }
                TabButton { text: "Duplicates"; font.pixelSize: 12; background: Rectangle { radius: 8; color: parent.checked ? theme.selection : "transparent" } }
                TabButton { text: "Health"; font.pixelSize: 12; background: Rectangle { radius: 8; color: parent.checked ? theme.selection : "transparent" } }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: topTabs.currentIndex

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: theme.rMd
                color: theme.surfaceRaised
                border.color: theme.border

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 10

                    Rectangle {
                        Layout.fillWidth: true
                        radius: 8
                        color: theme.surfaceMuted
                        border.color: theme.border
                        implicitHeight: 58
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 8
                            Text {
                                Layout.fillWidth: true
                                text: "Inbox is for newly downloaded documents. Use actions to place each file into your version workflow."
                                color: theme.textSecondary
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                font.pixelSize: 11
                            }
                            Text {
                                text: "Matched: " + root.inboxMatchedModel().length + "  |  Unmatched: " + root.inboxUnmatchedModel().length
                                color: theme.accent
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }

                    Text {
                        text: "Suggested Updates"
                        color: theme.textPrimary
                        font.pixelSize: 13
                        font.bold: true
                        visible: root.inboxMatchedModel().length > 0
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.inboxMatchedModel().length > 0 ? Math.min(260, contentHeight) : 0
                        visible: root.inboxMatchedModel().length > 0
                        model: root.inboxMatchedModel()
                        spacing: 8
                        clip: true
                        delegate: Rectangle {
                            required property var modelData
                            width: ListView.view.width
                            radius: theme.rMd
                            color: theme.surfaceMuted
                            border.color: theme.border
                            implicitHeight: row.implicitHeight + 16

                            RowLayout {
                                id: row
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 3
                                    Text { text: modelData.name; color: theme.textPrimary; font.bold: true; font.pixelSize: 13; elide: Text.ElideMiddle; Layout.fillWidth: true }
                                    Text { text: modelData.path; color: theme.textMuted; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }
                                    Text { text: "Group: " + modelData.matchedGroup + "  |  " + root.formatSize(modelData.size); color: theme.textSecondary; font.pixelSize: 11 }
                                    Text { text: root.inboxReason(modelData); color: theme.textMuted; font.pixelSize: 10; wrapMode: Text.WrapAtWordBoundaryOrAnywhere; Layout.fillWidth: true }
                                }

                                PrimaryActionButton {
                                    text: "Replace Current"
                                    enabled: !!(modelData.matchedGroup && modelData.matchedGroup.length > 0)
                                    onClicked: if (controller) controller.triageInbox(modelData.path, "replace")
                                }
                                PrimaryActionButton {
                                    text: "Add Version"
                                    enabled: !!(modelData.matchedGroup && modelData.matchedGroup.length > 0)
                                    onClicked: if (controller) controller.triageInbox(modelData.path, "version")
                                }
                                PrimaryActionButton {
                                    text: "Add As New"
                                    onClicked: if (controller) controller.triageInbox(modelData.path, "keep")
                                }
                                PrimaryActionButton {
                                    text: "Ignore"
                                    onClicked: if (controller) controller.triageInbox(modelData.path, "ignore")
                                }
                            }
                        }
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                    }

                    Text {
                        text: "Unmatched Downloads"
                        color: theme.textPrimary
                        font.pixelSize: 13
                        font.bold: true
                        visible: root.inboxUnmatchedModel().length > 0
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: root.inboxUnmatchedModel()
                        spacing: 8
                        clip: true
                        delegate: Rectangle {
                            required property var modelData
                            width: ListView.view.width
                            radius: theme.rMd
                            color: theme.surfaceMuted
                            border.color: theme.border
                            implicitHeight: row2.implicitHeight + 14

                            RowLayout {
                                id: row2
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 3
                                    Text { text: modelData.name; color: theme.textPrimary; font.bold: true; font.pixelSize: 13; elide: Text.ElideMiddle; Layout.fillWidth: true }
                                    Text { text: modelData.path; color: theme.textMuted; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }
                                    Text { text: root.formatSize(modelData.size); color: theme.textSecondary; font.pixelSize: 11 }
                                }

                                PrimaryActionButton { text: "Add As New"; onClicked: if (controller) controller.triageInbox(modelData.path, "keep") }
                                PrimaryActionButton { text: "Ignore"; onClicked: if (controller) controller.triageInbox(modelData.path, "ignore") }
                            }
                        }
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: theme.rMd
                color: theme.surfaceRaised
                border.color: theme.border

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 10

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 46
                        radius: 8
                        color: theme.surfaceMuted
                        border.color: theme.border

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            Text {
                                Layout.fillWidth: true
                                text: "Showing capped results for speed and readability. Noisy folders are excluded."
                                color: theme.textSecondary
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                            Text {
                                text: "Exact " + (controller ? controller.duplicateExact.length : 0) + "  |  Near " + (controller ? controller.duplicateNear.length : 0)
                                color: theme.accent
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }
                    }

                    TabBar {
                        id: dupesTabs
                        Layout.fillWidth: true
                        TabButton { text: qsTr("Exact (%1 shown)").arg(root.exactModel().length) }
                        TabButton { text: qsTr("Near (%1 shown)").arg(root.nearModel().length) }
                    }

                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: dupesTabs.currentIndex

                        ListView {
                            model: root.exactModel()
                            spacing: 8
                            clip: true

                            delegate: Rectangle {
                                required property var modelData
                                width: ListView.view.width
                                radius: theme.rSm
                                color: theme.surfaceMuted
                                border.color: theme.border
                                implicitHeight: content.implicitHeight + 14

                                ColumnLayout {
                                    id: content
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 6

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Text { text: "Duplicate Set"; color: theme.textPrimary; font.bold: true }
                                        Rectangle {
                                            radius: 10
                                            color: theme.selection
                                            implicitHeight: 22
                                            implicitWidth: 48
                                            Text { anchors.centerIn: parent; text: modelData.count; color: theme.textPrimary; font.pixelSize: 11; font.bold: true }
                                        }
                                    }

                                    Repeater {
                                        model: modelData.paths.slice(0, root.maxPathsPerExactSet)
                                        delegate: Text {
                                            required property var modelData
                                            text: "• " + modelData
                                            color: theme.textSecondary
                                            font.pixelSize: 11
                                            elide: Text.ElideMiddle
                                            Layout.fillWidth: true
                                        }
                                    }

                                    Text {
                                        visible: modelData.paths.length > root.maxPathsPerExactSet
                                        text: "+" + (modelData.paths.length - root.maxPathsPerExactSet) + " more files"
                                        color: theme.textMuted
                                        font.pixelSize: 10
                                    }

                                    RowLayout {
                                        PrimaryActionButton { text: "Archive Extras"; onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "archive") }
                                        PrimaryActionButton { text: "Keep Latest"; onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "keep_latest") }
                                        PrimaryActionButton { text: "Ignore Set"; onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "ignore_set") }
                                    }
                                }
                            }

                            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                        }

                        ListView {
                            model: root.nearModel()
                            spacing: 8
                            clip: true

                            delegate: Rectangle {
                                required property var modelData
                                width: ListView.view.width
                                radius: theme.rSm
                                color: theme.surfaceMuted
                                border.color: theme.border
                                implicitHeight: nearCol.implicitHeight + 14

                                ColumnLayout {
                                    id: nearCol
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 5

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Text {
                                            text: modelData.nameA + "  vs  " + modelData.nameB
                                            color: theme.textPrimary
                                            font.bold: true
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                        }
                                        Text {
                                            text: modelData.score + "%"
                                            color: theme.accent
                                            font.bold: true
                                            font.pixelSize: 12
                                        }
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        implicitHeight: 6
                                        radius: 3
                                        color: theme.surfaceRaised
                                        border.color: theme.border
                                        Rectangle {
                                            anchors.left: parent.left
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                            width: parent.width * Math.max(0, Math.min(1, Number(modelData.score || 0) / 100))
                                            radius: 3
                                            color: theme.warning
                                        }
                                    }

                                    Text { text: "A: " + modelData.pathA; color: theme.textSecondary; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }
                                    Text { text: "B: " + modelData.pathB; color: theme.textSecondary; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }

                                    RowLayout {
                                        PrimaryActionButton {
                                            text: "Set A Current"
                                            enabled: !!(controller && modelData.groupId)
                                            onClicked: if (controller && modelData.groupId) controller.setCurrentVersion(modelData.groupId, modelData.pathA)
                                        }
                                        PrimaryActionButton {
                                            text: "Set B Current"
                                            enabled: !!(controller && modelData.groupId)
                                            onClicked: if (controller && modelData.groupId) controller.setCurrentVersion(modelData.groupId, modelData.pathB)
                                        }
                                        PrimaryActionButton {
                                            text: "Archive A"
                                            onClicked: if (controller) controller.setDocumentStatus(modelData.pathA, "Archived")
                                        }
                                        PrimaryActionButton {
                                            text: "Archive B"
                                            onClicked: if (controller) controller.setDocumentStatus(modelData.pathB, "Archived")
                                        }
                                    }
                                }
                            }

                            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: theme.rMd
                color: theme.surfaceRaised
                border.color: theme.border

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 12

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 74
                        radius: theme.rMd
                        color: theme.surfaceMuted
                        border.color: theme.border

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 10
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                Text { text: "Overall Health"; color: theme.textSecondary; font.pixelSize: 11; font.capitalization: Font.AllUppercase; font.letterSpacing: 1.0 }
                                Text { text: root.healthLabel(root.metric("score", 0)); color: theme.textPrimary; font.pixelSize: 20; font.weight: Font.Bold }
                            }
                            Text {
                                text: root.metric("score", 0) + "/100"
                                color: root.metric("score", 0) >= 70 ? theme.success : theme.warning
                                font.pixelSize: 28
                                font.weight: Font.Bold
                            }
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: width > 900 ? 3 : 2
                        rowSpacing: 10
                        columnSpacing: 10

                        StatCard { Layout.fillWidth: true; title: "Documents"; value: String(root.metric("totalDocs", 0)); hint: "Tracked files"; accentColor: theme.accent }
                        StatCard { Layout.fillWidth: true; title: "Groups"; value: String(root.metric("groups", 0)); hint: "Version groups"; accentColor: theme.textSecondary }
                        StatCard { Layout.fillWidth: true; title: "Exact Duplicates"; value: String(root.metric("exactDuplicates", 0)); hint: "Byte-identical files"; accentColor: theme.warning }
                        StatCard { Layout.fillWidth: true; title: "Near Duplicates"; value: String(root.metric("nearDuplicates", 0)); hint: "Likely same content"; accentColor: theme.warning }
                        StatCard { Layout.fillWidth: true; title: "Drafts"; value: String(root.metric("drafts", 0)); hint: "Needs curation"; accentColor: theme.textSecondary }
                        StatCard { Layout.fillWidth: true; title: "Finals"; value: String(root.metric("finals", 0)); hint: "Completed docs"; accentColor: theme.success }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: theme.rMd
                        color: theme.surfaceMuted
                        border.color: theme.border

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 8

                            Text {
                                text: "Health Breakdown"
                                color: theme.textPrimary
                                font.bold: true
                                font.pixelSize: 13
                            }

                            MeterRow {
                                Layout.fillWidth: true
                                label: "Drafts"
                                valueText: root.metric("drafts", 0) + " / " + root.metric("totalDocs", 0)
                                ratio: root.pct(root.metric("drafts", 0), root.metric("totalDocs", 0))
                                fill: theme.warning
                            }
                            MeterRow {
                                Layout.fillWidth: true
                                label: "Finals"
                                valueText: root.metric("finals", 0) + " / " + root.metric("totalDocs", 0)
                                ratio: root.pct(root.metric("finals", 0), root.metric("totalDocs", 0))
                                fill: theme.success
                            }
                            MeterRow {
                                Layout.fillWidth: true
                                label: "Archived"
                                valueText: root.metric("archived", 0) + " / " + root.metric("totalDocs", 0)
                                ratio: root.pct(root.metric("archived", 0), root.metric("totalDocs", 0))
                                fill: theme.textSecondary
                            }
                            MeterRow {
                                Layout.fillWidth: true
                                label: "Exact Duplicate Share"
                                valueText: root.metric("exactDuplicates", 0) + " / " + (root.metric("exactDuplicates", 0) + root.metric("nearDuplicates", 0))
                                ratio: root.pct(root.metric("exactDuplicates", 0), root.metric("exactDuplicates", 0) + root.metric("nearDuplicates", 0))
                                fill: theme.accent
                            }
                            MeterRow {
                                Layout.fillWidth: true
                                label: "Near Duplicate Share"
                                valueText: root.metric("nearDuplicates", 0) + " / " + (root.metric("exactDuplicates", 0) + root.metric("nearDuplicates", 0))
                                ratio: root.pct(root.metric("nearDuplicates", 0), root.metric("exactDuplicates", 0) + root.metric("nearDuplicates", 0))
                                fill: theme.warning
                            }
                        }
                    }
                }
            }
        }
    }
}
