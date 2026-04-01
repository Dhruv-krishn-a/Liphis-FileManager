import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property string path: ""
    property var theme
    
    signal segmentClicked(string newPath)
    signal manualEntryRequested()

    color: "transparent"
    clip: true

    RowLayout {
        id: breadcrumbLayout
        anchors.fill: parent
        spacing: 0

        Repeater {
            model: root.pathSegments(root.path)
            
            delegate: RowLayout {
                spacing: 0
                
                ToolButton {
                    id: segmentButton
                    Layout.preferredHeight: 30
                    padding: 4
                    leftPadding: 8
                    rightPadding: 8
                    
                    background: Rectangle {
                        radius: 4
                        color: segmentButton.hovered ? root.theme.hover : "transparent"
                    }
                    
                    contentItem: Text {
                        text: modelData.name
                        color: segmentButton.hovered ? root.theme.accent : root.theme.textPrimary
                        font.pixelSize: 13
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: root.segmentClicked(modelData.path)
                }

                Icon {
                    name: "chevron-right"
                    iconSize: 12
                    color: root.theme.textTertiary
                    opacity: 0.5
                    visible: index < root.pathSegments(root.path).length - 1
                }
            }
        }
        
        Item { Layout.fillWidth: true }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.manualEntryRequested()
    }

    function pathSegments(p) {
        if (!p) return []
        let segments = []
        let parts = p.split("/")
        
        // Handle root
        segments.push({ name: "Root", path: "/" })
        
        let pathAcc = ""
        for (let i = 1; i < parts.length; i++) {
            if (parts[i].length === 0) continue
            
            pathAcc += "/" + parts[i]
            
            segments.push({
                name: parts[i],
                path: pathAcc
            })
        }
        return segments
    }
}
