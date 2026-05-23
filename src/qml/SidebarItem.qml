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

    contentItem: Item {
        anchors.fill: parent

        Item {
            id: iconContainer
            width: root.expanded ? (root.theme ? root.theme.iconMd : 16) : parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: root.expanded ? (root.theme ? root.theme.space12 : 12) : 0

            Icon {
                anchors.centerIn: parent
                name: root.iconName
                iconSize: root.expanded ? (root.theme ? root.theme.iconMd : 16) : (root.theme ? root.theme.iconLg : 20)
                color: root.active ? root.theme.accent : root.theme.textSecondary
            }
        }

        Text {
            visible: root.expanded
            text: root.text
            anchors.left: iconContainer.right
            anchors.leftMargin: root.theme ? root.theme.space8 : 8
            anchors.right: actionBtn.visible ? actionBtn.left : parent.right
            anchors.rightMargin: root.theme ? root.theme.space10 : 10
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            color: root.active ? root.theme.accent : root.theme.textPrimary
            font.pixelSize: root.theme ? root.theme.fontBody : 12
        }

        ThemedIconButton {
            id: actionBtn
            visible: root.showAction && root.expanded
            anchors.right: parent.right
            anchors.rightMargin: root.theme ? root.theme.space8 : 8
            anchors.verticalCenter: parent.verticalCenter
            theme: root.theme
            iconName: root.actionIconName
            iconSize: 12
            implicitWidth: 22
            implicitHeight: 22
            toolTip: root.actionToolTip
            onClicked: root.actionClicked()
        }
    }
}
