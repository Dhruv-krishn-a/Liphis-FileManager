import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property var theme
    property var controller
    property var toastManager
    property string section: "inbox"

    function sectionIndex(s) {
        if (s === "dupes") return 1
        if (s === "health") return 2
        return 0
    }

    function metric(key, fallback) {
        if (!controller || !controller.healthReport) return fallback
        var value = controller.healthReport[key]
        return value === undefined ? fallback : value
    }

    function statRows() {
        return [
            { title: "Health", value: metric("score", "--") + "/100", sub: "Score" },
            { title: "Indexed", value: metric("totalDocs", 0), sub: "Documents" },
            { title: "Duplicates", value: metric("exactDuplicates", 0) + " exact + " + metric("nearDuplicates", 0) + " near", sub: "Duplicate burden" },
            { title: "Inbox", value: metric("inbox", 0), sub: "New downloads" }
        ]
    }

    function inboxList() {
        return controller && controller.inboxItems ? controller.inboxItems : []
    }

    function duplicateList() {
        return controller && controller.duplicateExact ? controller.duplicateExact : []
    }

    Rectangle { anchors.fill: parent; color: theme ? theme.bg : "#f5f2ec" }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.space16 : 16
        spacing: theme ? theme.space12 : 12

        RowLayout {
            Layout.fillWidth: true
            spacing: theme ? theme.space8 : 8
            Repeater {
                model: statRows()
                delegate: Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 72
                    radius: theme ? theme.rMd : 10
                    color: theme ? theme.surfaceRaised : "#fff"
                    border.color: theme ? theme.border : "#ccc"
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 4
                        Text { text: modelData.title; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                        Text { text: modelData.value; color: theme ? theme.textPrimary : "#222"; font.pixelSize: 24; font.bold: true }
                        Text { text: modelData.sub; color: theme ? theme.textMuted : "#999"; font.pixelSize: 10 }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 38
            radius: theme ? theme.rMd : 10
            color: theme ? theme.surfaceMuted : "#eee"
            border.color: theme ? theme.border : "#ccc"
            TabBar {
                anchors.fill: parent
                anchors.margins: 4
                currentIndex: sectionIndex(section)
                onCurrentIndexChanged: {
                    if (currentIndex === 0) section = "inbox"
                    else if (currentIndex === 1) section = "dupes"
                    else section = "health"
                }
                TabButton { text: "Inbox" }
                TabButton { text: "Duplicates" }
                TabButton { text: "Health" }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: sectionIndex(section)

            // Inbox tab
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: theme ? theme.rMd : 10
                color: theme ? theme.surfaceRaised : "#fff"
                border.color: theme ? theme.border : "#ccc"
                anchors.margins: 0
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8
                    Text { text: "Inbox"; font.pixelSize: 14; font.bold: true; color: theme ? theme.textPrimary : "#222" }
                    Text { text: "Review downloaded files, accept tracked ones or ignore the rest."; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: inboxList()
                        spacing: 6
                        delegate: Rectangle {
                            width: ListView.view.width
                            radius: theme ? theme.rSm : 8
                            color: theme ? theme.surfaceMuted : "#f9f6ef"
                            border.color: theme ? theme.border : "#ddd"
                            implicitHeight: 96
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 4
                                Text { text: modelData.name; font.bold: true; color: theme ? theme.textPrimary : "#222"; elide: Text.ElideMiddle }
                                Text { text: modelData.path; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11; elide: Text.ElideMiddle }
                                RowLayout {
                                    spacing: 8
                                    Button { text: "Accept"; onClicked: if (controller) controller.triageInbox(modelData.path, "keep") }
                                    Button { text: "Ignore"; onClicked: if (controller) controller.triageInbox(modelData.path, "ignore") }
                                }
                            }
                        }
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                    }
                }
            }

            // Duplicates tab
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: theme ? theme.rMd : 10
                color: theme ? theme.surfaceRaised : "#fff"
                border.color: theme ? theme.border : "#ccc"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6
                    Text { text: "Duplicates"; font.pixelSize: 14; font.bold: true; color: theme ? theme.textPrimary : "#222" }
                    Text { text: "Exact match groups. Keep one copy or archive extras."; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: duplicateList()
                        spacing: 6
                        delegate: Rectangle {
                            width: ListView.view.width
                            radius: theme ? theme.rSm : 8
                            color: theme ? theme.surfaceMuted : "#f9f6ef"
                            border.color: theme ? theme.border : "#ddd"
                            implicitHeight: content.implicitHeight + 12
                            ColumnLayout {
                                id: content
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 6
                                RowLayout { Layout.fillWidth: true; spacing: 6
                                    Text { text: "Set"; color: theme ? theme.textSecondary : "#666" }
                                    Text { text: "(" + modelData.count + ")"; color: theme ? theme.accent : "#b86" }
                                    Item { Layout.fillWidth: true }
                                    Button { text: "Keep latest"; onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "keep_latest") }
                                }
                                Repeater {
                                    model: modelData.paths
                                    delegate: Text { text: modelData; color: theme ? theme.textSecondary : "#555"; font.pixelSize: 11; elide: Text.ElideMiddle }
                                }
                            }
                        }
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                    }
                }
            }

            // Health tab
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: theme ? theme.rMd : 10
                color: theme ? theme.surfaceRaised : "#fff"
                border.color: theme ? theme.border : "#ccc"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 10
                    Text { text: "Health"; font.pixelSize: 14; font.bold: true; color: theme ? theme.textPrimary : "#222" }
                    Text { text: "Score and health summary."; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 60
                        radius: theme ? theme.rSm : 6
                        color: theme ? theme.surfaceMuted : "#eee"
                        border.color: theme ? theme.border : "#ccc"
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 4
                            Text { text: "Score"; font.pixelSize: 11; color: theme ? theme.textSecondary : "#666" }
                            Text { text: metric("score", "--") + "/100"; font.pixelSize: 28; color: theme ? theme.textPrimary : "#222"; font.bold: true }
                        }
                    }
                    RowLayout { spacing: 12
                        ColumnLayout { Layout.fillWidth: true; spacing: 2
                            Text { text: "Drafts"; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                            Rectangle { Layout.fillWidth: true; implicitHeight: 8; radius: 4; color: theme ? theme.surfaceMuted : "#eee"; Rectangle { anchors.fill: parent; width: parent.width * Math.min(1, metric("drafts", 0)/Math.max(1, metric("totalDocs", 1))); color: theme ? theme.warning : "#c89" } }
                            Text { text: metric("drafts", 0) + " drafts"; color: theme ? theme.textMuted : "#999"; font.pixelSize: 10 }
                        }
                        ColumnLayout { Layout.fillWidth: true; spacing: 2
                            Text { text: "Finals"; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                            Rectangle { Layout.fillWidth: true; implicitHeight: 8; radius: 4; color: theme ? theme.surfaceMuted : "#eee"; Rectangle { anchors.fill: parent; width: parent.width * Math.min(1, metric("finals", 0)/Math.max(1, metric("totalDocs", 1))); color: theme ? theme.success : "#5a9" } }
                            Text { text: metric("finals", 0) + " finals"; color: theme ? theme.textMuted : "#999"; font.pixelSize: 10 }
                        }
                    }
                    Text { text: "Next step: process inbox entries and archive old duplicates."; color: theme ? theme.textSecondary : "#666"; font.pixelSize: 11 }
                }
            }
        }
    }
}
