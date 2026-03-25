import QtQuick
import QtQuick.Controls

Button {
    id: root

    property var theme
    property bool selected: false

    flat: true
    implicitHeight: 24
    padding: 0

    contentItem: Text {
        text: root.text
        color: root.selected ? root.theme.accent : root.theme.textSecondary
        font.pixelSize: root.theme ? root.theme.fontBody - 1 : 11
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    background: Rectangle {
        radius: root.theme ? root.theme.rSm : 8
        color: root.selected
            ? (root.theme ? root.theme.selection : "transparent")
            : (root.hovered ? (root.theme ? root.theme.hover : "transparent") : "transparent")
        border.width: root.selected ? 1 : 0
        border.color: root.theme ? root.theme.borderStrong : "transparent"
    }
}
