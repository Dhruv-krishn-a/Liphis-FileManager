import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: root
    height: 36
    contentHeight: 36
    clip: true
    
    // Horizontal scroll for long paths
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    property string path: ""
    signal pathClicked(string newPath)

    RowLayout {
        spacing: 4
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 12
        
        // Root folder button
        Button {
            text: "Root"
            flat: true
            font.pixelSize: 12; font.weight: (path === "/" || path === "") ? Font.Bold : Font.Normal
            palette.buttonText: (path === "/" || path === "") ? "#EAEAFF" : "#94a3b8"
            onClicked: root.pathClicked("/")
            background: Rectangle {
                color: parent.hovered ? Qt.rgba(1, 1, 1, 0.08) : "transparent"
                radius: 6
            }
        }
        
        Text {
            text: "❯"
            color: Qt.rgba(148/255.0, 163/255.0, 184/255.0, 0.4)
            font.pixelSize: 10
            visible: path.split("/").filter(s => s !== "").length > 0
        }
        
        Repeater {
            model: path.split("/").filter(s => s !== "")
            
            RowLayout {
                spacing: 4
                
                Button {
                    text: modelData
                    flat: true
                    font.pixelSize: 12; font.weight: index === (path.split("/").filter(s => s !== "").length - 1) ? Font.Bold : Font.Normal
                    palette.buttonText: index === (path.split("/").filter(s => s !== "").length - 1) ? "#EAEAFF" : "#94a3b8"
                    
                    onClicked: {
                        var parts = path.split("/")
                        var target = ""
                        var count = 0
                        var targetIndex = index + 1
                        for(var i=0; i<parts.length; i++) {
                           if (parts[i] === "" && i > 0) continue
                           if (parts[i] !== "") count++
                           target += (i === 0 ? "" : "/") + parts[i]
                           if (count === targetIndex) break
                        }
                        root.pathClicked(target === "" ? "/" : target)
                    }
                    
                    background: Rectangle {
                        color: parent.hovered ? Qt.rgba(1, 1, 1, 0.08) : "transparent"
                        radius: 6
                    }
                }
                
                Text {
                    text: "❯"
                    color: Qt.rgba(148/255.0, 163/255.0, 184/255.0, 0.4)
                    font.pixelSize: 10
                    visible: index < (path.split("/").filter(s => s !== "").length - 1)
                }
            }
        }
    }
}
