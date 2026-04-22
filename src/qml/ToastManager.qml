import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import "."
import liphis

Item {
    id: root
    anchors.fill: parent
    z: 1000 // Always on top
    enabled: false // Let clicks pass through

    property bool darkMode: false
    AppTheme { id: theme; isDark: root.darkMode }

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
                width: Math.min(400, msgText.implicitWidth + 40)
                height: 32
                radius: 16
                color: theme.surfaceElevated
                border.color: model.isError ? theme.error : theme.accent
                border.width: 1
                
                layer.enabled: true
                layer.effect: DropShadow {
                    radius: 6
                    color: model.isError
                        ? Qt.rgba(theme.error.r, theme.error.g, theme.error.b, 0.15)
                        : Qt.rgba(theme.accent.r, theme.accent.g, theme.accent.b, 0.10)
                    samples: 10
                    verticalOffset: 1
                }

                RowLayout {
                    anchors.centerIn: parent; spacing: 8
                    Icon { 
                        name: model.isError ? "close" : "info"
                        iconSize: 14; color: model.isError ? theme.error : theme.accent
                    }
                    Text {
                        id: msgText
                        text: model.message
                        color: theme.textPrimary; font.pixelSize: 12; font.weight: Font.Normal
                    }
                }
                
                opacity: 0
                Component.onCompleted: opacity = 1
                Behavior on opacity { NumberAnimation { duration: 300 } }
            }
        }
    }
}
