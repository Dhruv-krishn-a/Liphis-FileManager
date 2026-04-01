import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property string title: ""
    property string field: ""
    property bool isSortActive: false
    property bool sortAscending: true
    property var theme
    property alias columnWidth: root.width
    property int minWidth: 40
    property int horizontalAlignment: Text.AlignLeft
    property int paddingLeft: 0
    property int paddingRight: 0
    property bool showSplitter: true

    signal clicked()
    signal widthChangedByHandle(int newWidth)

    height: parent.height
    color: "transparent"
    
    Layout.preferredWidth: width

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.horizontalAlignment === Text.AlignRight ? 4 : root.paddingLeft
        anchors.rightMargin: root.horizontalAlignment === Text.AlignRight ? root.paddingRight : (showSplitter ? 4 : 0)
        spacing: 4
        LayoutMirroring.enabled: root.horizontalAlignment === Text.AlignRight
        LayoutMirroring.childrenInherit: true

        Text {
            id: titleText
            text: root.title
            color: root.isSortActive ? root.theme.accent : root.theme.textSecondary
            font.bold: root.isSortActive
            font.pixelSize: 11
            elide: Text.ElideRight
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: root.horizontalAlignment
        }

        Icon {
            visible: root.isSortActive
            name: root.sortAscending ? "arrow-up" : "arrow-down"
            iconSize: 10
            color: root.theme.accent
            Layout.alignment: Qt.AlignVCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        anchors.rightMargin: showSplitter ? 8 : 0
        onClicked: root.clicked()
    }

    // Resize Handle
    Rectangle {
        visible: root.showSplitter
        anchors.right: parent.right
        width: 1
        height: parent.height * 0.6
        anchors.verticalCenter: parent.verticalCenter
        color: root.theme.border
        opacity: 0.5
    }

    MouseArea {
        visible: root.showSplitter
        anchors.right: parent.right
        anchors.rightMargin: -4
        width: 8
        height: parent.height
        cursorShape: Qt.SizeHorCursor
        
        property int startX: 0
        property int startWidth: 0

        onPressed: (mouse) => {
            startX = mouse.x
            startWidth = root.width
        }

        onPositionChanged: (mouse) => {
            if (pressed) {
                let delta = mouse.x - startX
                let newW = Math.max(root.minWidth, startWidth + delta)
                root.widthChangedByHandle(newW)
            }
        }
    }
}
