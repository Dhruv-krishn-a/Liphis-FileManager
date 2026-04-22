import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Duplicates Center"
    modal: true
    width: 780
    height: 580
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    standardButtons: Dialog.Close

    property var theme
    property var controller

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
            implicitHeight: 66
            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                Text {
                    Layout.fillWidth: true
                    text: "Review exact and near duplicates, then archive extras safely."
                    color: theme.textSecondary
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                }
                Text {
                    text: controller ? ((controller.duplicateExact.length + controller.duplicateNear.length) + " groups") : "0 groups"
                    color: theme.accent
                    font.pixelSize: 14
                    font.bold: true
                }
            }
        }

        TabBar {
            id: tabs
            Layout.fillWidth: true
            TabButton { text: qsTr("Exact (%1)").arg(controller ? controller.duplicateExact.length : 0) }
            TabButton { text: qsTr("Likely Same (%1)").arg(controller ? controller.duplicateNear.length : 0) }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabs.currentIndex

            ListView {
                model: controller ? controller.duplicateExact : []
                spacing: 8
                clip: true
                delegate: Rectangle {
                    required property var modelData
                    width: ListView.view.width
                    color: theme.surfaceMuted
                    border.color: theme.border
                    radius: theme.rSm
                    implicitHeight: col.implicitHeight + 16

                    ColumnLayout {
                        id: col
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 8
                        Text { text: "Duplicate Set (" + modelData.count + ")"; color: theme.textPrimary; font.pixelSize: 12; font.bold: true }
                        Repeater {
                            model: modelData.paths
                            delegate: Text { required property var modelData; text: "• " + modelData; color: theme.textSecondary; font.pixelSize: 11; elide: Text.ElideMiddle }
                        }
                        RowLayout {
                            Button {
                                text: "Mark extras Archived"
                                onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "archive")
                                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                            }
                            Button {
                                text: "Keep Latest"
                                onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "keep_latest")
                                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                            }
                            Button {
                                text: "Ignore Set"
                                onClicked: if (controller) controller.mergeExactDuplicateGroup(modelData.hashKey, "ignore_set")
                                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
                            }
                        }
                    }
                }
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            }

            ListView {
                model: controller ? controller.duplicateNear : []
                spacing: 8
                clip: true
                delegate: Rectangle {
                    required property var modelData
                    width: ListView.view.width
                    color: theme.surfaceMuted
                    border.color: theme.border
                    radius: theme.rSm
                    implicitHeight: 82

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 4
                        Text { text: modelData.groupId + " • Similarity " + modelData.score + "%"; color: theme.textPrimary; font.pixelSize: 12; font.bold: true }
                        Text { text: "A: " + modelData.pathA; color: theme.textSecondary; font.pixelSize: 11; elide: Text.ElideMiddle }
                        Text { text: "B: " + modelData.pathB; color: theme.textSecondary; font.pixelSize: 11; elide: Text.ElideMiddle }
                    }
                }
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            }
        }
    }
}
