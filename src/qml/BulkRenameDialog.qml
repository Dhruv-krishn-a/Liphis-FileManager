import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import liphis

Dialog {
    id: root
    title: "Bulk Rename"
    width: 600; height: 500
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }

    property var paths: []
    property alias prefix: prefixField.text
    property alias suffix: suffixField.text
    property alias find: findField.text
    property alias replace: replaceField.text

    signal renameRequested(var paths, string prefix, string suffix, string find, string replace)

    ColumnLayout {
        anchors.fill: parent; spacing: theme.space16
        
        Text { text: "Selected: " + paths.length + " items"; font.bold: true; color: theme.textPrimary; font.pixelSize: theme.fontBody }

        GridLayout {
            columns: 2; columnSpacing: theme.space12; rowSpacing: theme.space12; Layout.fillWidth: true
            
            Label { text: "Add Prefix:"; color: theme.textSecondary }
            TextField { id: prefixField; Layout.fillWidth: true; placeholderText: "e.g. 2024_" }

            Label { text: "Add Suffix:"; color: theme.textSecondary }
            TextField { id: suffixField; Layout.fillWidth: true; placeholderText: "e.g. _final" }

            Label { text: "Find:"; color: theme.textSecondary }
            TextField { id: findField; Layout.fillWidth: true; placeholderText: "text to find" }

            Label { text: "Replace:"; color: theme.textSecondary }
            TextField { id: replaceField; Layout.fillWidth: true; placeholderText: "replacement text" }
        }

        Text { text: "Preview:"; font.bold: true; Layout.topMargin: theme.space12; color: theme.textPrimary }
        
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true; clip: true
            model: paths
            delegate: Text {
                width: parent.width
                text: {
                    var name = modelData.split('/').pop()
                    var newName = name
                    if (find !== "" && newName.includes(find)) newName = newName.replace(find, replace)
                    return name + " -> " + prefix + newName + suffix
                }
                elide: Text.ElideMiddle; font.pixelSize: theme.fontBody - 1; color: theme.textSecondary
            }
        }
    }

    onAccepted: renameRequested(paths, prefix, suffix, find, replace)
}
