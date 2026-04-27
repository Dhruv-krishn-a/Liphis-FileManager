import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 400
    height: 400
    visible: true

    ListView {
        id: lv
        anchors.fill: parent
        model: 1000
        delegate: Text {
            text: "Item " + modelData
            font.pixelSize: 20
            height: 40
        }
        
        WheelHandler {
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                console.log("angleDelta.y:", event.angleDelta.y);
                lv.flick(0, event.angleDelta.y * 15);
                event.accepted = true;
            }
        }
    }
}
