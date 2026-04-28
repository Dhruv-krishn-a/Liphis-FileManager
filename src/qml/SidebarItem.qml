import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ItemDelegate {
    id: root

    property var theme
    property string iconName: "folder"
    property bool active: false
    property bool showAction: false
    property string actionIconName: "close"
    property string actionToolTip: ""
    property bool expanded: true

    signal actionClicked()
    signal rightClicked()

    Layout.fillWidth: true
    height: 32
    padding: 0
    ToolTip.visible: hovered && !expanded
    ToolTip.text: root.text

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: (mouse) => {
            if (mouse.button === Qt.RightButton) {
                root.rightClicked()
            }
        }
    }

    background: Rectangle {
        anchors.fill: parent
        anchors.leftMargin: root.theme ? root.theme.space8 : 8
        anchors.rightMargin: root.theme ? root.theme.space8 : 8
        radius: root.theme ? root.theme.rSm : 8
        color: root.active
            ? (root.theme ? root.theme.selection : "transparent")
            : (root.hovered ? (root.theme ? root.theme.hover : "transparent") : "transparent")
        border.width: root.active ? 1 : 0
        border.color: root.theme ? root.theme.border : "transparent"
    }

    contentItem: RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.expanded ? (root.theme ? root.theme.space12 : 12) : 0
        anchors.rightMargin: root.expanded ? (root.theme ? root.theme.space10 : 10) : 0
        spacing: root.theme ? root.theme.space8 : 8

        Item {
            Layout.fillWidth: false
            Layout.preferredWidth: root.expanded ? (root.theme ? root.theme.iconMd : 16) : (root.theme ? root.theme.iconLg + 2 : 20)
            Layout.preferredHeight: root.expanded ? (root.theme ? root.theme.iconMd : 16) : (root.theme ? root.theme.iconLg + 2 : 20)
            Icon {
                anchors.centerIn: parent
                name: root.iconName
                iconSize: root.expanded ? (root.theme ? root.theme.iconMd : 16) : (root.theme ? root.theme.iconLg : 18)
                color: root.active ? root.theme.accent : root.theme.textSecondary
            }
        }

        Text {
            visible: root.expanded
            text: root.text
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            elide: Text.ElideRight
            color: root.active ? root.theme.accent : root.theme.textPrimary
            font.pixelSize: root.theme ? root.theme.fontBody : 12
            verticalAlignment: Text.AlignVCenter
        }

        ThemedIconButton {
            visible: root.showAction && root.expanded
            theme: root.theme
            iconName: root.actionIconName
            iconSize: 12
            implicitWidth: 22
            implicitHeight: 22
            toolTip: root.actionToolTip
            onClicked: {
                root.actionClicked()
            }
        }
    }
}
