import QtQuick
import QtQuick.Shapes

Item {
    id: root
    property bool isDir: false
    property string iconName: ""
    
    // Matte Folder Icon
    Rectangle {
        anchors.fill: parent
        visible: isDir
        color: "transparent"

        // Back tab
        Rectangle {
            x: 0; y: parent.height * 0.15
            width: parent.width * 0.45; height: parent.height * 0.25
            radius: 4
            color: "#8A867E" // Muted text/border color as base
        }

        // Main body
        Rectangle {
            x: 0; y: parent.height * 0.3
            width: parent.width; height: parent.height * 0.7
            radius: 6
            color: "#B5B1AA" // Secondary text color as base
            
            // Subtle top highlight
            Rectangle {
                anchors.top: parent.top; width: parent.width; height: 1
                color: Qt.rgba(1, 1, 1, 0.15); radius: 1
            }
        }
    }

    // Matte File Icon
    Rectangle {
        anchors.fill: parent
        anchors.margins: parent.width * 0.15
        visible: !isDir
        color: "#2A2A2A" // elevated surface
        border.color: "#404040" // strong border
        border.width: 1
        radius: 4

        // Folded corner
        Item {
            anchors.right: parent.right; anchors.top: parent.top
            width: parent.width * 0.3; height: width
            
            // Background mask for corner
            Rectangle {
                anchors.fill: parent
                color: "transparent" // Let parent's background show through
            }
            
            // The fold triangle
            Shape {
                anchors.fill: parent
                ShapePath {
                    fillColor: "#404040" // border strong
                    strokeColor: "transparent"
                    startX: 0; startY: 0
                    PathLine { x: parent.width; y: parent.height }
                    PathLine { x: 0; y: parent.height }
                    PathLine { x: 0; y: 0 }
                }
            }
        }
        
        // Internal matte lines
        Column {
            anchors.centerIn: parent; anchors.verticalCenterOffset: 4; spacing: 4
            Repeater {
                model: 3
                Rectangle { width: root.width * 0.35; height: 2; color: "#404040"; radius: 1 }
            }
        }
    }
}
