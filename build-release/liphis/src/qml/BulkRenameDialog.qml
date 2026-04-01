import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Qt5Compat.GraphicalEffects

Dialog {
    id: root
    title: "Bulk Rename"
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: 650
    height: 520
    standardButtons: Dialog.Ok | Dialog.Cancel
    anchors.centerIn: parent

    property var theme
    property var controller
    property var files: []
    
    property string search: searchField.text
    property string replace: replaceField.text
    property string prefix: prefixField.text
    property string suffix: suffixField.text

    background: Rectangle {
        color: theme.surfaceRaised
        radius: 12
        border.color: theme.border
    }

    function show(selectedFiles) {
        files = selectedFiles
        open()
    }

    function previewName(oldName) {
        let newName = oldName
        if (search.length > 0) {
            newName = newName.split(search).join(replace)
        }
        return prefix + newName + suffix
    }

    contentItem: ColumnLayout {
        spacing: 16
        anchors.margins: 20

        Text {
            text: "RENAME " + root.files.length + " ITEMS"
            color: theme.accent
            font.pixelSize: 11
            font.bold: true
            font.letterSpacing: 1.2
        }

        GridLayout {
            columns: 2
            rowSpacing: 12
            columnSpacing: 16
            Layout.fillWidth: true

            Label { text: "Search For:"; color: theme.textSecondary; font.pixelSize: 12 }
            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Pattern to find..."
                selectByMouse: true
            }

            Label { text: "Replace With:"; color: theme.textSecondary; font.pixelSize: 12 }
            TextField {
                id: replaceField
                Layout.fillWidth: true
                placeholderText: "Replacement text..."
                selectByMouse: true
            }

            Label { text: "Prefix:"; color: theme.textSecondary; font.pixelSize: 12 }
            TextField {
                id: prefixField
                Layout.fillWidth: true
                placeholderText: "Add to beginning..."
                selectByMouse: true
            }

            Label { text: "Suffix:"; color: theme.textSecondary; font.pixelSize: 12 }
            TextField {
                id: suffixField
                Layout.fillWidth: true
                placeholderText: "Add to end..."
                selectByMouse: true
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: theme.surface
            radius: 8
            border.color: theme.border

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 1
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    height: 32
                    color: theme.surfaceMuted
                    radius: 8
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        Text { text: "Original Name"; Layout.fillWidth: true; color: theme.textTertiary; font.bold: true; font.pixelSize: 10 }
                        Text { text: "New Name"; Layout.fillWidth: true; color: theme.textTertiary; font.bold: true; font.pixelSize: 10 }
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: root.files
                    delegate: RowLayout {
                        width: parent.width
                        height: 32
                        spacing: 12
                        Layout.leftMargin: 12
                        Layout.rightMargin: 12

                        Text {
                            text: modelData.name
                            Layout.fillWidth: true
                            color: theme.textSecondary
                            font.pixelSize: 12
                            elide: Text.ElideRight
                        }

                        Icon {
                            name: "arrow-right"
                            iconSize: 14
                            color: theme.textTertiary
                            opacity: 0.5
                        }

                        Text {
                            text: root.previewName(modelData.name)
                            Layout.fillWidth: true
                            color: theme.accent
                            font.pixelSize: 12
                            font.bold: true
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }

    onAccepted: {
        let paths = []
        for (let i = 0; i < files.length; i++) {
            paths.push(files[i].path)
        }
        controller.bulkRename(paths, prefix, suffix, search, replace)
    }
}
