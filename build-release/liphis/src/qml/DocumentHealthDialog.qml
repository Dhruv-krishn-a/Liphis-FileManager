import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Document Health Dashboard"
    modal: true
    width: 560
    height: 520
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    standardButtons: Dialog.Close

    property var theme
    property var controller

    signal openInboxRequested()
    signal openDuplicatesRequested()
    signal openNamingRequested()

    background: Rectangle {
        color: theme.surfaceRaised
        border.color: theme.border
        radius: theme.rMd
    }

    contentItem: ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme.space16
        spacing: theme.space12

        Rectangle {
            Layout.fillWidth: true
            radius: theme.rMd
            color: theme.surfaceMuted
            border.color: theme.border
            implicitHeight: 92

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 4
                Text { text: "Organization Score"; color: theme.textSecondary; font.pixelSize: 11 }
                Text {
                    text: controller ? (controller.healthReport.score + " / 100") : "--"
                    color: theme.accent
                    font.pixelSize: 28
                    font.bold: true
                }
                Text {
                    text: controller && controller.healthReport && controller.healthReport.label
                          ? controller.healthReport.label : ""
                    color: theme.textPrimary
                    font.pixelSize: 12
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 8
            columnSpacing: 8

            Repeater {
                model: [
                    { k: "Documents", v: controller ? controller.healthReport.totalDocs : 0 },
                    { k: "Groups", v: controller ? controller.healthReport.groups : 0 },
                    { k: "Exact Duplicates", v: controller ? controller.healthReport.exactDuplicates : 0 },
                    { k: "Near Duplicates", v: controller ? controller.healthReport.nearDuplicates : 0 },
                    { k: "Drafts", v: controller ? controller.healthReport.drafts : 0 },
                    { k: "Inbox Pending", v: controller ? controller.healthReport.inbox : 0 }
                ]
                delegate: Rectangle {
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: 58
                    radius: theme.rSm
                    color: theme.surfaceMuted
                    border.color: theme.border
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        Text { text: modelData.k; color: theme.textSecondary; Layout.fillWidth: true; font.pixelSize: 11; wrapMode: Text.WordWrap }
                        Text { text: String(modelData.v); color: theme.textPrimary; font.bold: true; font.pixelSize: 16 }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            radius: theme.rSm
            color: theme.surfaceMuted
            border.color: theme.border
            implicitHeight: 64

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                Text {
                    Layout.fillWidth: true
                    text: "Use these actions to clean up your docs quickly."
                    color: theme.textSecondary
                    font.pixelSize: 12
                }
                RowLayout {
                    spacing: 8
                    Button { text: "Inbox"; onClicked: root.openInboxRequested() }
                    Button { text: "Duplicates"; onClicked: root.openDuplicatesRequested() }
                    Button { text: "Naming"; onClicked: root.openNamingRequested() }
                }
            }
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Button {
                text: "Open Inbox"
                Layout.fillWidth: true
                onClicked: root.openInboxRequested()
                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
            }
            Button {
                text: "Open Duplicates"
                Layout.fillWidth: true
                onClicked: root.openDuplicatesRequested()
                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
            }
            Button {
                text: "Open Naming"
                Layout.fillWidth: true
                onClicked: root.openNamingRequested()
                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
            }
        }
    }
}
