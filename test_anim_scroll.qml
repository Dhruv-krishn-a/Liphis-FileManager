import QtQuick
import QtQuick.Controls
import QtQuick.Window

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

        PropertyAnimation {
            id: scrollAnim
            target: lv
            property: "contentY"
            duration: 150
            easing.type: Easing.OutSine
        }

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                if (event.angleDelta.y !== 0) {
                    let target = lv.contentY;
                    if (scrollAnim.running) target = scrollAnim.to;
                    target -= (event.angleDelta.y / 120) * 200; // scroll 200px per notch
                    target = Math.max(lv.originY, Math.min(target, lv.originY + lv.contentHeight - lv.height));
                    
                    if (target !== lv.contentY) {
                        scrollAnim.to = target;
                        scrollAnim.restart();
                    }
                    event.accepted = true;
                }
            }
        }
    }
}
