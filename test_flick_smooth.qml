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
        
        flickDeceleration: 800
        maximumFlickVelocity: 4000

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                lv.flick(0, event.angleDelta.y * 15);
                event.accepted = true;
            }
        }
    }
}
