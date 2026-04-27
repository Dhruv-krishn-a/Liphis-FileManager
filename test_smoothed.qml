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

        property real targetContentY: contentY

        SmoothedAnimation {
            id: smoothAnim
            target: lv
            property: "contentY"
            to: lv.targetContentY
            duration: 150
            velocity: -1
        }

        onMovementEnded: targetContentY = contentY
        onDragEnded: targetContentY = contentY
        onFlickEnded: targetContentY = contentY

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                let step = event.angleDelta.y * 3;
                let newTarget = lv.targetContentY - step;
                newTarget = Math.max(lv.originY, Math.min(newTarget, lv.originY + lv.contentHeight - lv.height));
                
                lv.targetContentY = newTarget;
                smoothAnim.restart(); // SmoothedAnimation restarts smoothly
                event.accepted = true;
            }
        }
    }
}
