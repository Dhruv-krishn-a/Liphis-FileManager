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

        // Behavior on contentY is the standard way to get smooth wheel scrolling without kinetic physics
        Behavior on contentY {
            NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
        }

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                if (event.angleDelta.y !== 0) {
                    let step = (event.angleDelta.y / 120) * 120; // 3 rows * 40px
                    lv.contentY = Math.max(lv.originY, Math.min(lv.originY + lv.contentHeight - lv.height, lv.contentY - step));
                    event.accepted = true;
                }
            }
        }
    }
}
