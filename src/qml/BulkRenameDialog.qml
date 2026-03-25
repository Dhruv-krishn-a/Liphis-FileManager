import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Bulk Rename"
    width: 600; height: 500
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    property var paths: []
    property alias prefix: prefixField.text
    property alias suffix: suffixField.text
    property alias find: findField.text
    property alias replace: replaceField.text

    signal renameRequested(var paths, string prefix, string suffix, string find, string replace)

    ColumnLayout {
        anchors.fill: parent; spacing: 15
        
        Text { text: "Selected: " + paths.length + " items"; font.bold: true }

        GridLayout {
            columns: 2; columnSpacing: 10; rowSpacing: 10; Layout.fillWidth: true
            
            Label { text: "Add Prefix:" }
            TextField { id: prefixField; Layout.fillWidth: true; placeholderText: "e.g. 2024_" }

            Label { text: "Add Suffix:" }
            TextField { id: suffixField; Layout.fillWidth: true; placeholderText: "e.g. _final" }

            Label { text: "Find:" }
            TextField { id: findField; Layout.fillWidth: true; placeholderText: "text to find" }

            Label { text: "Replace:" }
            TextField { id: replaceField; Layout.fillWidth: true; placeholderText: "replacement text" }
        }

        Text { text: "Preview:"; font.bold: true; Layout.topMargin: 10 }
        
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
                elide: Text.ElideMiddle; font.pixelSize: 11; color: "#444"
            }
        }
    }

    onAccepted: renameRequested(paths, prefix, suffix, find, replace)
}
