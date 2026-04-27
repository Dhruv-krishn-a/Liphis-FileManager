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
        
        property real targetY: contentY

        Behavior on targetY {
            NumberAnimation { duration: 150; easing.type: Easing.OutSine }
        }

        onTargetYChanged: {
            if (!dragging && !flicking) {
                contentY = targetY;
            }
        }

        onMovementEnded: targetY = contentY
        onDragEnded: targetY = contentY

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse
            onWheel: (event) => {
                lv.targetY = Math.max(lv.originY, Math.min(lv.targetY - (event.angleDelta.y/120) * 120, lv.originY + lv.contentHeight - lv.height));
                event.accepted = true;
            }
        }
    }
}
