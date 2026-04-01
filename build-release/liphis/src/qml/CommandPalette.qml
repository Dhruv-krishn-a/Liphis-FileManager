import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Liphis.Core 1.0

Popup {
    id: root
    
    property var theme: null
    
    modal: true
    focus: true
    padding: 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    
    x: Math.round((parent.width - width) / 2)
    y: 80
    
    width: Math.min(680, parent.width - 40)
    height: Math.min(520, parent.height - 120)

    // Entrance / Exit Animations
    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 120; easing.type: Easing.OutQuad }
        NumberAnimation { property: "y"; from: 65; to: 80; duration: 220; easing.type: Easing.OutBack }
        NumberAnimation { property: "scale"; from: 0.98; to: 1.0; duration: 200; easing.type: Easing.OutBack }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 80; easing.type: Easing.InQuad }
        NumberAnimation { property: "y"; from: 80; to: 88; duration: 180; easing.type: Easing.InQuad }
        NumberAnimation { property: "scale"; from: 1.0; to: 0.98; duration: 180; easing.type: Easing.InQuad }
    }

    background: Rectangle {
        radius: theme.rMd || 12
        color: theme.surfaceRaised || "#ffffff"
        border.color: theme.borderStrong || "#e0e0e0"
        border.width: 1
        
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            horizontalOffset: 0
            verticalOffset: 8
            radius: 24
            samples: 32
            color: Qt.rgba(0,0,0,0.2)
        }
    }

    onOpened: {
        commandInput.text = ""
        commandInput.forceActiveFocus()
        resultsList.currentIndex = 0
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Search Input Header
        Rectangle {
            Layout.fillWidth: true
            height: 64
            color: "transparent"
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 16
                
                Icon {
                    name: "search"
                    iconSize: 20
                    color: theme.accent
                }

                TextField {
                    id: commandInput
                    Layout.fillWidth: true
                    placeholderText: "What do you want to do?"
                    font.pixelSize: 18
                    color: theme.textPrimary
                    placeholderTextColor: theme.textPlaceholder
                    background: null
                    selectByMouse: true
                    
                    onTextChanged: {
                        commandManager.filter = text
                        resultsList.currentIndex = 0
                    }

                    Keys.onPressed: (event) => {
                        if (event.key === Qt.Key_Down) {
                            resultsList.currentIndex = (resultsList.currentIndex + 1) % resultsList.count
                            event.accepted = true
                        } else if (event.key === Qt.Key_Up) {
                            resultsList.currentIndex = (resultsList.currentIndex - 1 + resultsList.count) % resultsList.count
                            event.accepted = true
                        } else if (event.key === Qt.Key_PageDown) {
                            resultsList.currentIndex = Math.min(resultsList.count - 1, resultsList.currentIndex + 6)
                            event.accepted = true
                        } else if (event.key === Qt.Key_PageUp) {
                            resultsList.currentIndex = Math.max(0, resultsList.currentIndex - 6)
                            event.accepted = true
                        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                            if (resultsList.count > 0) {
                                commandManager.executeCommand(resultsList.currentIndex)
                                root.close()
                            }
                            event.accepted = true
                        }
                    }
                }
                
                // Shortcut hint for ESC
                Rectangle {
                    color: theme.surfaceMuted
                    radius: 4
                    width: 34
                    height: 24
                    border.color: theme.border
                    Text {
                        anchors.centerIn: parent
                        text: "ESC"
                        color: theme.textTertiary
                        font.pixelSize: 9
                        font.weight: Font.Bold
                    }
                }
            }
            
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: theme.border
            }
        }

        // Results List
        ListView {
            id: resultsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: commandManager.model
            boundsBehavior: Flickable.StopAtBounds
            
            highlightMoveDuration: 150
            highlightResizeDuration: 150
            
            section.property: "category"
            section.criteria: ViewSection.FullString
            section.delegate: Rectangle {
                width: resultsList.width
                height: 32
                color: "transparent"
                
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 6
                    text: section.toUpperCase()
                    color: theme.accent
                    font.pixelSize: 11
                    font.weight: Font.Bold
                    font.letterSpacing: 1.2
                    opacity: 0.8
                }
                
                Rectangle {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom
                    height: 1
                    color: theme.accent
                    opacity: 0.1
                }
            }

            delegate: ItemDelegate {
                id: delegateItem
                width: resultsList.width
                height: 52
                highlighted: ListView.isCurrentItem
                
                property string category: model.category

                onClicked: {
                    commandManager.executeCommand(index)
                    root.close()
                }

                background: Rectangle {
                    anchors.fill: parent
                    anchors.margins: 4
                    radius: 8
                    color: highlighted ? theme.selection : "transparent"
                    
                    Rectangle {
                        visible: highlighted
                        anchors.left: parent.left
                        width: 4
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        radius: 2
                        color: theme.accent
                    }
                }

                contentItem: RowLayout {
                    spacing: 16
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 6
                        color: highlighted ? Qt.rgba(theme.accent.r, theme.accent.g, theme.accent.b, 0.1) : "transparent"
                        
                        Icon {
                            anchors.centerIn: parent
                            name: model.icon || "command"
                            iconSize: 20
                            color: highlighted ? theme.accent : theme.textSecondary
                            opacity: highlighted ? 1.0 : 0.8
                        }
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: model.label
                        color: highlighted ? theme.textPrimary : theme.textSecondary
                        font.pixelSize: 15
                        font.weight: highlighted ? Font.Medium : Font.Normal
                        elide: Text.ElideRight
                    }
                    
                    // Shortcut Badge
                    Row {
                        spacing: 4
                        visible: model.shortcut.length > 0
                        Layout.alignment: Qt.AlignRight
                        
                        property var keys: model.shortcut.split("+")
                        
                        Repeater {
                            model: parent.keys
                            Rectangle {
                                color: highlighted ? theme.accent : theme.surfaceMuted
                                radius: 4
                                implicitWidth: Math.max(kText.implicitWidth + 10, 22)
                                implicitHeight: 22
                                border.color: highlighted ? "transparent" : theme.border
                                border.width: 1
                                
                                Text {
                                    id: kText
                                    anchors.centerIn: parent
                                    text: modelData
                                    color: highlighted ? "white" : theme.textSecondary
                                    font.pixelSize: 10
                                    font.weight: Font.DemiBold
                                }
                            }
                        }
                    }
                }
            }
            
            // Empty State
            ColumnLayout {
                anchors.centerIn: parent
                visible: resultsList.count === 0
                spacing: 16
                
                Rectangle {
                    width: 64; height: 64; radius: 32; color: theme.surfaceMuted
                    Layout.alignment: Qt.AlignHCenter
                    Icon {
                        anchors.centerIn: parent
                        name: "search"
                        iconSize: 32
                        color: theme.textMuted
                        opacity: 0.5
                    }
                }
                
                Text {
                    text: "No matches found for \"" + commandInput.text + "\""
                    color: theme.textMuted
                    font.pixelSize: 16
                    Layout.alignment: Qt.AlignHCenter
                }
                
                Text {
                    text: "Try searching for a different keyword"
                    color: theme.textTertiary
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
        
        // Footer hint
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: theme.surfaceMuted
            visible: resultsList.count > 0
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                
                Row {
                    spacing: 12
                    
                    Row {
                        spacing: 6
                        Rectangle { width: 18; height: 18; radius: 3; color: theme.surfaceRaised; border.color: theme.border; Text { anchors.centerIn: parent; text: "↑↓"; color: theme.textTertiary; font.pixelSize: 10 } }
                        Text { text: "Navigate"; color: theme.textTertiary; font.pixelSize: 11; anchors.verticalCenter: parent.verticalCenter }
                    }
                    
                    Row {
                        spacing: 6
                        Rectangle { width: 18; height: 18; radius: 3; color: theme.surfaceRaised; border.color: theme.border; Text { anchors.centerIn: parent; text: "↵"; color: theme.textTertiary; font.pixelSize: 12 } }
                        Text { text: "Execute"; color: theme.textTertiary; font.pixelSize: 11; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                Text {
                    text: resultsList.count + " results"
                    color: theme.textTertiary
                    font.pixelSize: 11
                }
            }
        }
    }
}
