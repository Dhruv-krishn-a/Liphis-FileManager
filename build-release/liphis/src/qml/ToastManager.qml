import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import "."

Item {
    id: root
    anchors.fill: parent
    z: 1000 // Always on top
    enabled: false // Let clicks pass through

    ListModel { id: toastModel }

    function show(message, isError) {
        toastModel.append({ "message": message, "isError": isError || false })
        hideTimer.start()
    }

    Timer {
        id: hideTimer
        interval: 3500
        repeat: true
        running: toastModel.count > 0
        onTriggered: {
            if (toastModel.count > 0) toastModel.remove(0)
            else stop()
        }
    }

    ColumnLayout {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 60
        spacing: 12

        Repeater {
            model: toastModel
            delegate: Rectangle {
                id: toastRect
                width: Math.min(500, msgText.implicitWidth + 60)
                height: 48
                radius: 24
                color: Qt.rgba(15/255.0, 18/255.0, 38/255.0, 0.95)
                border.color: model.isError ? "#ef4444" : "#7C7CFF"
                border.width: 1
                
                layer.enabled: true
                layer.effect: DropShadow { radius: 15; color: model.isError ? Qt.rgba(239/255.0, 68/255.0, 68/255.0, 0.3) : Qt.rgba(124/255.0, 124/255.0, 255/255.0, 0.3); samples: 16 }

                RowLayout {
                    anchors.centerIn: parent; spacing: 10
                    Icon { 
                        name: model.isError ? "close" : "info"
                        size: 18; color: model.isError ? "#ef4444" : "#7C7CFF"
                    }
                    Text {
                        id: msgText
                        text: model.message
                        color: "#EAEAFF"; font.pixelSize: 13; font.weight: Font.Medium
                    }
                }
                
                opacity: 0
                Component.onCompleted: opacity = 1
                Behavior on opacity { NumberAnimation { duration: 300 } }
            }
        }
    }
}
