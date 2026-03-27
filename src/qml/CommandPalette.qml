import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
    
    width: Math.min(650, parent.width - 40)
    height: Math.min(500, parent.height - 120)

    // Entrance / Exit Animations
    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 150; easing.type: Easing.OutQuad }
        NumberAnimation { property: "y"; from: 60; to: 80; duration: 250; easing.type: Easing.OutBack }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100; easing.type: Easing.InQuad }
        NumberAnimation { property: "y"; from: 80; to: 90; duration: 200; easing.type: Easing.InQuad }
    }

    background: Rectangle {
        radius: theme.rMd
        color: theme.surfaceRaised
        border.color: theme.borderStrong
        border.width: 1
        
        // Shadow (Simplified)
        Rectangle {
            anchors.fill: parent
            anchors.margins: -1
            radius: theme.rMd
            color: "transparent"
            border.color: Qt.rgba(0,0,0,0.15)
            border.width: 1
            z: -1
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
            height: 60
            color: "transparent"
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 14
                
                Icon {
                    name: "search"
                    size: 20
                    color: theme.accent
                }

                TextField {
                    id: commandInput
                    Layout.fillWidth: true
                    placeholderText: "What do you want to do?"
                    font.pixelSize: 16
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
                            resultsList.currentIndex = Math.min(resultsList.count - 1, resultsList.currentIndex + 5)
                            event.accepted = true
                        } else if (event.key === Qt.Key_PageUp) {
                            resultsList.currentIndex = Math.max(0, resultsList.currentIndex - 5)
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
                
                // Active Category Badge (if any)
                Rectangle {
                    visible: resultsList.count > 0
                    color: theme.surfaceMuted
                    radius: 4
                    implicitWidth: catLabel.implicitWidth + 12
                    implicitHeight: 24
                    Text {
                        id: catLabel
                        anchors.centerIn: parent
                        text: resultsList.currentItem ? resultsList.currentItem.category : ""
                        color: theme.textSecondary
                        font.pixelSize: 10
                        font.weight: Font.DemiBold
                        font.capitalization: Font.AllUppercase
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
            
            highlightMoveDuration: 120
            highlightResizeDuration: 120
            
            delegate: ItemDelegate {
                id: delegateItem
                width: resultsList.width
                height: 48
                highlighted: ListView.isCurrentItem
                
                property string category: model.category

                onClicked: {
                    commandManager.executeCommand(index)
                    root.close()
                }

                background: Rectangle {
                    color: highlighted ? theme.selection : "transparent"
                    
                    Rectangle {
                        visible: highlighted
                        anchors.left: parent.left
                        width: 4
                        height: parent.height
                        color: theme.accent
                    }
                }

                contentItem: RowLayout {
                    spacing: 16
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    
                    Icon {
                        name: model.icon || "command"
                        size: 20
                        color: highlighted ? theme.accent : theme.textSecondary
                        opacity: highlighted ? 1.0 : 0.7
                    }
                    
                    ColumnLayout {
                        spacing: 1
                        Layout.fillWidth: true
                        Text {
                            text: model.label
                            color: highlighted ? theme.textPrimary : theme.textSecondary
                            font.pixelSize: 14
                            font.weight: highlighted ? Font.Medium : Font.Normal
                            elide: Text.ElideRight
                        }
                        Text {
                            text: model.category
                            color: highlighted ? theme.textSecondary : theme.textTertiary
                            font.pixelSize: 11
                            visible: !highlighted // Only show category on non-highlighted for cleaner look
                        }
                    }
                    
                    // Shortcut Badge
                    Row {
                        spacing: 4
                        visible: model.shortcut.length > 0
                        
                        property var keys: model.shortcut.split("+")
                        
                        Repeater {
                            model: parent.keys
                            Rectangle {
                                color: highlighted ? theme.accent : theme.surfaceMuted
                                radius: 4
                                implicitWidth: kText.implicitWidth + 8
                                implicitHeight: 20
                                border.color: highlighted ? "transparent" : theme.border
                                border.width: 1
                                
                                Text {
                                    id: kText
                                    anchors.centerIn: parent
                                    text: modelData
                                    color: highlighted ? "white" : theme.textSecondary
                                    font.pixelSize: 9
                                    font.weight: Font.Bold
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
                spacing: 12
                Icon {
                    name: "search"
                    size: 48
                    color: theme.textMuted
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    text: "No results found for \"" + commandInput.text + "\""
                    color: theme.textMuted
                    font.pixelSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
        
        // Footer hint
        Rectangle {
            Layout.fillWidth: true
            height: 32
            color: theme.surfaceMuted
            visible: resultsList.count > 0
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                
                Text {
                    text: "Use ↑↓ to navigate, ↵ to execute"
                    color: theme.textTertiary
                    font.pixelSize: 11
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
