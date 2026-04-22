import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Canonical Naming Assistant"
    modal: true
    width: 840
    height: 620
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    standardButtons: Dialog.Close

    property var theme
    property var controller
    property var activeController
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
            implicitHeight: 74
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 4
                Text { text: "STANDARDIZE DOCUMENT NAMES"; color: theme.textMuted; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.0 }
                Text { text: "Preview first, then apply. Undo restores the last batch."; color: theme.textSecondary; font.pixelSize: 12 }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: templateField
                Layout.fillWidth: true
                text: controller ? controller.defaultNameTemplate() : "{base}_{date}_v{version}"
                placeholderText: "{base}_{date}_v{version}"
                background: Rectangle {
                    radius: 8
                    border.color: theme.border
                    color: theme.surfaceRaised
                }
            }
            Button {
                text: "Preview"
                onClicked: {
                    if (!controller) return
                    const p = activeController ? activeController.currentPath : ""
                    controller.previewCanonicalNames(p, templateField.text)
                }
                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
            }
            Button {
                text: "Apply"
                onClicked: {
                    if (!controller) return
                    const n = controller.applyCanonicalNames()
                    if (toastManager) toastManager.show(qsTr("Renamed %1 files").arg(n), false)
                }
                background: Rectangle { radius: 6; color: parent.hovered ? theme.accentSoft : theme.surfaceRaised; border.color: theme.border }
            }
            Button {
                text: "Undo Last"
                onClicked: {
                    if (!controller) return
                    const n = controller.undoLastCanonicalRename()
                    if (toastManager) toastManager.show(qsTr("Undo restored %1 files").arg(n), false)
                }
                background: Rectangle { radius: 6; color: parent.hovered ? theme.hover : theme.surfaceRaised; border.color: theme.border }
            }
        }

        Text {
            text: "Tokens: {base} {date} {version} {ext}"
            color: theme.textMuted
            font.pixelSize: 11
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 6
            model: controller ? controller.namingPreview : []
            delegate: Rectangle {
                required property var modelData
                width: ListView.view.width
                color: modelData.conflict ? Qt.rgba(theme.error.r, theme.error.g, theme.error.b, 0.12) : theme.surfaceMuted
                border.color: modelData.conflict ? theme.error : theme.border
                radius: theme.rSm
                implicitHeight: 66

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 3
                    Text { text: modelData.oldName + "  ->  " + modelData.newName; color: theme.textPrimary; font.pixelSize: 12; font.bold: true; elide: Text.ElideMiddle; Layout.fillWidth: true }
                    Text { text: modelData.path; color: theme.textMuted; font.pixelSize: 10; elide: Text.ElideMiddle; Layout.fillWidth: true }
                    Text { visible: modelData.conflict; text: "Conflict: target file exists"; color: theme.error; font.pixelSize: 10 }
                }
            }
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }
    }
}
