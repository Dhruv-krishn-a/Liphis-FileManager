import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root

    property var theme
    property string title: ""
    default property alias sectionContent: contentColumn.data

    spacing: root.theme.space8

    Text {
        visible: root.title.length > 0
        text: root.title
        color: root.theme.textMuted
        font.pixelSize: root.theme.fontLabel
        font.bold: true
        font.letterSpacing: root.theme.letterSpacingLabel
        Layout.leftMargin: root.theme.space2
    }

    ColumnLayout {
        id: contentColumn
        Layout.fillWidth: true
        spacing: root.theme.space8
    }
}
