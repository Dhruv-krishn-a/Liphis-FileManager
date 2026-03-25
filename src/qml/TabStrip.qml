import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var tabModel
    property int currentIndex: 0

    signal tabSelected(int index)
    signal tabClosed(int index)
    signal tabMoved(int from, int to)
    signal homeRequested()

    height: 38
    color: theme.surface

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ListView {
            id: tabList
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: ListView.Horizontal
            model: root.tabModel
            spacing: theme ? theme.space4 : 4
            clip: true
            interactive: true

            delegate: Rectangle {
                id: tabItem
                width: 172
                height: tabList.height
                radius: theme ? theme.rSm : 8
                color: root.currentIndex === index ? theme.surfaceRaised : "transparent"
                property real originalX: 0
                property bool dragged: false

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 2
                    color: theme.accent
                    visible: root.currentIndex === index
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: theme ? theme.space10 : 10
                    anchors.rightMargin: theme ? theme.space4 : 4
                    spacing: theme ? theme.space6 : 6

                    Text {
                        text: model.title
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        color: root.currentIndex === index ? theme.textPrimary : theme.textSecondary
                        font.pixelSize: theme ? theme.fontBody : 12
                        verticalAlignment: Text.AlignVCenter
                    }

                    ToolButton {
                        Layout.preferredWidth: 24
                        Layout.preferredHeight: 24
                        flat: true
                        padding: 0
                        contentItem: Item {
                            implicitWidth: 12
                            implicitHeight: 12
                            Icon { anchors.centerIn: parent; name: "close"; size: 12; color: theme.textMuted }
                        }
                        onClicked: root.tabClosed(index)
                        background: Rectangle {
                            radius: theme ? theme.rSm : 8
                            color: parent.hovered ? theme.hover : "transparent"
                        }
                    }
                }

                MouseArea {
                    id: tabMouse
                    anchors.fill: parent
                    anchors.rightMargin: 30
                    drag.target: tabItem
                    drag.axis: Drag.XAxis
                    onPressed: {
                        tabItem.originalX = tabItem.x
                        tabItem.dragged = false
                    }
                    onPositionChanged: {
                        if (pressed && Math.abs(tabItem.x - tabItem.originalX) > 4) tabItem.dragged = true
                    }
                    onReleased: {
                        if (tabItem.dragged) {
                            var distance = tabItem.x - tabItem.originalX
                            var threshold = tabItem.width * 0.35
                            var targetIndex = index
                            if (distance > threshold) targetIndex = Math.min(index + 1, root.tabModel.count - 1)
                            else if (distance < -threshold) targetIndex = Math.max(index - 1, 0)
                            if (targetIndex !== index) root.tabMoved(index, targetIndex)
                        } else {
                            root.tabSelected(index)
                        }
                        tabItem.x = tabItem.originalX
                    }
                }
            }
        }

        ThemedIconButton {
            theme: root.theme
            iconName: "home"
            iconSize: theme ? theme.iconMd : 16
            implicitWidth: 40
            implicitHeight: 40
            onClicked: root.homeRequested()
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: theme.border
    }
}
