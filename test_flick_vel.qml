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
                let addition = event.angleDelta.y * 15;
                lv.flick(0, lv.verticalVelocity + addition);
                event.accepted = true;
            }
        }
    }
}
