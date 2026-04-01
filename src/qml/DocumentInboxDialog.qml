import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Document Inbox"
    modal: true
    width: 760
    height: 560
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    standardButtons: Dialog.Close

    property var theme
    property var controller
    property var toastManager

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
            color: theme.surfaceMuted
            border.color: theme.border
            radius: theme.rMd
            implicitHeight: 82

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        text: "DOWNLOAD TRIAGE"
                        color: theme.textMuted
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 1.0
                    }
                    Text {
                        text: controller ? qsTr("%1 files need decision").arg(controller.inboxItems.length) : ""
                        color: theme.textPrimary
                        font.pixelSize: 16
                        font.bold: true
                    }
                    Text {
                        text: "Replace current, add as version, keep separate, or ignore."
                        color: theme.textSecondary
                        font.pixelSize: 11
                    }
                }

                Button {
                    text: "Refresh"
                    onClicked: if (controller) controller.scanDownloadsInbox()
                    background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                }
                Button {
                    text: "Clear"
                    onClicked: if (controller) controller.clearInbox()
                    background: Rectangle { radius: 6; color: parent.hovered ? Qt.rgba(theme.error.r, theme.error.g, theme.error.b, 0.08) : theme.surfaceRaised; border.color: theme.border }
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: theme.space8
            model: controller ? controller.inboxItems : []

            delegate: Rectangle {
                required property var modelData
                width: ListView.view.width
                color: theme.surfaceMuted
                border.color: theme.border
                radius: theme.rSm
                implicitHeight: row.implicitHeight + 16

                RowLayout {
                    id: row
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 12

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Text { text: modelData.name; color: theme.textPrimary; font.pixelSize: 13; font.bold: true; elide: Text.ElideMiddle; Layout.fillWidth: true }
                        Text { text: modelData.path; color: theme.textMuted; font.pixelSize: 10; elide: Text.ElideMiddle; Layout.fillWidth: true }

                        RowLayout {
                            spacing: 6
                            Rectangle {
                                radius: 6
                                color: Qt.rgba(theme.accent.r, theme.accent.g, theme.accent.b, 0.12)
                                border.color: Qt.rgba(theme.accent.r, theme.accent.g, theme.accent.b, 0.28)
                                implicitHeight: 20
                                implicitWidth: confidenceText.implicitWidth + 12
                                Text {
                                    id: confidenceText
                                    anchors.centerIn: parent
                                    text: modelData.matchedGroup && modelData.matchedGroup.length > 0 ? (modelData.confidence + "% match") : "No match"
                                    color: theme.accent
                                    font.pixelSize: 10
                                    font.bold: true
                                }
                            }
                            Text {
                                text: modelData.matchedGroup && modelData.matchedGroup.length > 0
                                      ? ("Suggested: " + modelData.suggestion + " • " + modelData.matchedGroup)
                                      : "Suggested: keep separate"
                                color: theme.textSecondary
                                font.pixelSize: 10
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 6
                        Button {
                            text: "Replace"
                            enabled: !!(modelData.matchedGroup && modelData.matchedGroup.length > 0)
                            onClicked: if (controller) controller.triageInbox(modelData.path, "replace")
                            background: Rectangle { radius: 6; color: parent.hovered ? theme.accentSoft : theme.surfaceRaised; border.color: theme.border }
                        }
                        Button {
                            text: "Version"
                            enabled: !!(modelData.matchedGroup && modelData.matchedGroup.length > 0)
                            onClicked: if (controller) controller.triageInbox(modelData.path, "version")
                            background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                        }
                    }
                    ColumnLayout {
                        spacing: 6
                        Button {
                            text: "Keep"
                            onClicked: if (controller) controller.triageInbox(modelData.path, "keep")
                            background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                        }
                        Button {
                            text: "Ignore"
                            onClicked: if (controller) controller.triageInbox(modelData.path, "ignore")
                            background: Rectangle { radius: 6; color: parent.hovered ? Qt.rgba(theme.error.r, theme.error.g, theme.error.b, 0.08) : theme.surfaceRaised; border.color: theme.border }
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }
    }
}
