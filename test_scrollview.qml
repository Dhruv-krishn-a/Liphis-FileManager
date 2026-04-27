import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 400
    height: 400
    visible: true

    ScrollView {
        anchors.fill: parent
        
        ListView {
            model: 1000
            delegate: Text {
                text: "Item " + modelData
                font.pixelSize: 20
                height: 40
            }
        }
    }
}
