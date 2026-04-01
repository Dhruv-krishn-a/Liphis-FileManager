import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import liphis

Dialog {
    id: root
    title: "Open With..."
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: 450
    height: 600
    anchors.centerIn: parent
    standardButtons: Dialog.Cancel

    property string targetPath: ""
    property var controller: null
    property var theme
    property var apps: []
    property bool showingAll: false
    property string filterText: ""

    function show(path, ctrl) {
        targetPath = path
        controller = ctrl
        showingAll = false
        filterText = ""
        apps = controller.getAssociatedApps(path)
        open()
    }

    background: Rectangle {
        color: theme.surfaceRaised
        radius: 12
        border.color: theme.border
    }

    header: Rectangle {
        height: 50
        color: "transparent"
        Text {
            anchors.centerIn: parent
            text: showingAll ? "ALL APPLICATIONS" : "OPEN WITH"
            color: theme.textPrimary
            font.pixelSize: 14; font.bold: true; font.letterSpacing: 1.1
        }
    }

    contentItem: ColumnLayout {
        spacing: 15
        
        Text {
            text: showingAll ? "Select any application to open this file:" : "Choose a recommended application:"
            color: theme.textSecondary
            font.pixelSize: 12
            Layout.fillWidth: true
        }

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "Search applications..."
            visible: showingAll
            text: filterText
            onTextChanged: filterText = text
            background: Rectangle {
                radius: 8
                color: theme.surfaceSecondary
                border.color: parent.activeFocus ? theme.accent : theme.border
            }
        }

        ListView {
            id: appList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: root.apps.filter(function(app) {
                if (!filterText) return true
                return app.name.toLowerCase().includes(filterText.toLowerCase()) || 
                       app.id.toLowerCase().includes(filterText.toLowerCase())
            })
            clip: true
            spacing: 4
            
            delegate: ItemDelegate {
                width: appList.width
                height: 54
                padding: 10
                
                contentItem: RowLayout {
                    spacing: 12
                    Icon {
                        name: modelData.icon || "application-x-executable"
                        iconSize: 32
                        color: theme.textPrimary
                    }
                    ColumnLayout {
                        spacing: 2
                        Layout.fillWidth: true
                        Text { 
                            text: modelData.name
                            color: theme.textPrimary
                            font.pixelSize: 13
                            font.bold: true 
                        }
                        Text { 
                            text: modelData.comment || modelData.exec
                            color: theme.textSecondary
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }
                    Rectangle {
                        visible: modelData.isDefault || false
                        width: 50
                        height: 20
                        radius: 10
                        color: theme.accent
                        opacity: 0.1
                        Text {
                            anchors.centerIn: parent
                            text: "DEFAULT"
                            color: theme.accent
                            font.pixelSize: 9
                            font.bold: true
                        }
                    }
                }
                
                onClicked: {
                    if (setAsDefaultCheck.checked) {
                        let mime = controller.getMimeType(root.targetPath)
                        controller.setDefaultApp(mime, modelData.id)
                    }
                    controller.openWithApp(root.targetPath, modelData.exec)
                    root.close()
                }
                
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? theme.hover : "transparent"
                }
            }
            
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        CheckBox {
            id: setAsDefaultCheck
            text: "Set as default application for this file type"
            Layout.fillWidth: true
            font.pixelSize: 11
            checked: false
            contentItem: Text {
                text: parent.text
                font: parent.font
                color: theme.textSecondary
                leftPadding: parent.indicator.width + parent.spacing
                verticalAlignment: Text.AlignVCenter
            }
        }
        
        Button {
            text: showingAll ? "Back to Recommended" : "Other Application..."
            Layout.fillWidth: true
            onClicked: {
                if (showingAll) {
                    showingAll = false
                    apps = controller.getAssociatedApps(targetPath)
                } else {
                    showingAll = true
                    apps = controller.getAllApplications()
                }
            }
        }
    }
}
