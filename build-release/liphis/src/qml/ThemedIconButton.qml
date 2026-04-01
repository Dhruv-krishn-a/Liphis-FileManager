import QtQuick
import QtQuick.Controls

ToolButton {
    id: root

    property var theme
    property string iconName: ""
    property int iconSize: theme ? theme.iconMd : 16
    property bool accentWhenChecked: false
    property string toolTip: ""

    implicitWidth: theme ? theme.controlMd : 38
    implicitHeight: theme ? theme.controlMd : 38
    padding: 0
    ToolTip.visible: hovered && toolTip.length > 0
    ToolTip.text: toolTip

    contentItem: Item {
        implicitWidth: root.iconSize
        implicitHeight: root.iconSize
        Icon {
            anchors.centerIn: parent
            name: root.iconName
            iconSize: root.iconSize
            color: root.enabled
                ? (root.accentWhenChecked && root.checked ? root.theme.accent : root.theme.textPrimary)
                : root.theme.textMuted
        }
    }

    background: Rectangle {
        radius: root.theme ? root.theme.rSm : 8
        color: root.down
            ? (root.theme ? root.theme.pressed : "transparent")
            : ((root.hovered || root.checked) ? (root.theme ? root.theme.hover : "transparent") : "transparent")
        border.width: (root.hovered || root.checked) ? 1 : 0
        border.color: root.theme ? root.theme.border : "transparent"
    }
}
