import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtCore

Dialog {
    id: root
    title: "Settings"
    modal: true
    closePolicy: Popup.CloseOnEscape
    visible: false
    width: 580
    height: 640
    standardButtons: Dialog.Close
    anchors.centerIn: parent

    property var theme
    property alias viewSettings: viewSettings
    property alias generalSettings: generalSettings

    Settings {
        id: viewSettings
        category: "ListView"
        property bool showSize: true
        property bool showType: true
        property bool showDate: true
        property bool showPermissions: false
        property bool showOwner: false
        property bool showGroup: false
        property bool showMime: false
        property bool showCreated: false
        property bool showAccessed: false

        property int colNameWidth: 300
        property int colSizeWidth: 100
        property int colTypeWidth: 140
        property int colDateWidth: 180
        property int colCreatedWidth: 180
        property int colAccessedWidth: 180
        property int colPermsWidth: 100
        property int colOwnerWidth: 100
        property int colGroupWidth: 100
        property int colMimeWidth: 180

        property string sortField: "name"
        property bool sortAscending: true
        property string defaultViewMode: "list"
    }

    Settings {
        id: generalSettings
        category: "General"
        property bool singleClick: false
        property bool restoreSession: true
        property bool showAnimations: true
        property bool confirmDelete: true
        property bool showHiddenByDefault: false
        property int defaultZoom: 100
        property string iconTheme: "outline"
    }

    background: Rectangle {
        color: theme.surfaceRaised
        radius: 12
        border.color: theme.border
        layer.enabled: true
        layer.effect: DropShadow {
            radius: 8
            color: Qt.rgba(0,0,0,0.15)
            samples: 16
            verticalOffset: 2
        }
    }

    header: Rectangle {
        height: 60
        color: "transparent"
        Text {
            anchors.centerIn: parent
            text: "SETTINGS"
            color: theme.textPrimary
            font.pixelSize: 16
            font.bold: true
            font.letterSpacing: 1.2
        }
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: theme.border
        }
    }

    contentItem: ColumnLayout {
        spacing: 0
        anchors.margins: 0

        TabBar {
            id: settingsTabs
            Layout.fillWidth: true
            background: Rectangle { color: theme.surfaceMuted }
            
            TabButton {
                id: generalTab
                text: "GENERAL"
                font.pixelSize: 11; font.bold: true
                contentItem: Text {
                    text: generalTab.text
                    color: generalTab.checked ? theme.accent : theme.textSecondary
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: "transparent"
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 2; color: theme.accent; visible: generalTab.checked }
                }
            }
            TabButton {
                id: viewTab
                text: "VIEW & COLUMNS"
                font.pixelSize: 11; font.bold: true
                contentItem: Text {
                    text: viewTab.text
                    color: viewTab.checked ? theme.accent : theme.textSecondary
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: "transparent"
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 2; color: theme.accent; visible: viewTab.checked }
                }
            }
            TabButton {
                id: behaviorTab
                text: "BEHAVIOR"
                font.pixelSize: 11; font.bold: true
                contentItem: Text {
                    text: behaviorTab.text
                    color: behaviorTab.checked ? theme.accent : theme.textSecondary
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: "transparent"
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 2; color: theme.accent; visible: behaviorTab.checked }
                }
            }
        }

        StackLayout {
            currentIndex: settingsTabs.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // General Settings
            ScrollView {
                clip: true
                ColumnLayout {
                    width: parent.width; spacing: 16; anchors.margins: 20

                    SectionHeader { label: "Appearance" }
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Theme Mode"; color: theme.textPrimary; Layout.fillWidth: true }
                        RowLayout {
                            spacing: 8
                            Button {
                                text: "Light"; checkable: true; checked: !theme.isDark
                                onClicked: theme.isDark = false
                                flat: true
                            }
                            Button {
                                text: "Dark"; checkable: true; checked: theme.isDark
                                onClicked: theme.isDark = true
                                flat: true
                            }
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Icon Style"; color: theme.textPrimary; Layout.fillWidth: true }
                        ComboBox {
                            model: ["outline", "filled"]
                            currentIndex: generalSettings.iconTheme === "filled" ? 1 : 0
                            onActivated: generalSettings.iconTheme = currentValue
                            Layout.preferredWidth: 120
                        }
                    }

                    SectionHeader { label: "Startup" }
                    SettingsSwitch { theme: root.theme; label: "Restore last session"; checked: generalSettings.restoreSession; onToggled: generalSettings.restoreSession = checked }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Default View Mode"; color: theme.textPrimary; Layout.fillWidth: true }
                        ComboBox {
                            model: ["list", "grid", "tree"]
                            currentIndex: ["list", "grid", "tree"].indexOf(viewSettings.defaultViewMode)
                            onActivated: viewSettings.defaultViewMode = currentValue
                            Layout.preferredWidth: 120
                        }
                    }
                }
            }

            // View Settings
            ScrollView {
                clip: true
                ColumnLayout {
                    width: parent.width; spacing: 12; anchors.margins: 20
                    SectionHeader { label: "List View Columns" }
                    SettingsSwitch { theme: root.theme; label: "Size"; checked: viewSettings.showSize; onToggled: viewSettings.showSize = checked }
                    SettingsSwitch { theme: root.theme; label: "Type"; checked: viewSettings.showType; onToggled: viewSettings.showType = checked }
                    SettingsSwitch { theme: root.theme; label: "Date Modified"; checked: viewSettings.showDate; onToggled: viewSettings.showDate = checked }
                    SettingsSwitch { theme: root.theme; label: "Permissions"; checked: viewSettings.showPermissions; onToggled: viewSettings.showPermissions = checked }
                    SettingsSwitch { theme: root.theme; label: "Owner"; checked: viewSettings.showOwner; onToggled: viewSettings.showOwner = checked }
                    SettingsSwitch { theme: root.theme; label: "Group"; checked: viewSettings.showGroup; onToggled: viewSettings.showGroup = checked }
                    SettingsSwitch { theme: root.theme; label: "MIME Type"; checked: viewSettings.showMime; onToggled: viewSettings.showMime = checked }
                    SettingsSwitch { theme: root.theme; label: "Date Created"; checked: viewSettings.showCreated; onToggled: viewSettings.showCreated = checked }
                    SettingsSwitch { theme: root.theme; label: "Date Accessed"; checked: viewSettings.showAccessed; onToggled: viewSettings.showAccessed = checked }
                }
            }

            // Behavior Settings
            ScrollView {
                clip: true
                ColumnLayout {
                    width: parent.width; spacing: 16; anchors.margins: 20
                    SectionHeader { label: "File Operations" }
                    SettingsSwitch { theme: root.theme; label: "Single click to open"; checked: generalSettings.singleClick; onToggled: generalSettings.singleClick = checked }
                    SettingsSwitch { theme: root.theme; label: "Confirm before deleting"; checked: generalSettings.confirmDelete; onToggled: generalSettings.confirmDelete = checked }
                    SettingsSwitch { theme: root.theme; label: "Show hidden files by default"; checked: generalSettings.showHiddenByDefault; onToggled: generalSettings.showHiddenByDefault = checked }
                    
                    SectionHeader { label: "Performance" }
                    SettingsSwitch { theme: root.theme; label: "Enable animations"; checked: generalSettings.showAnimations; onToggled: generalSettings.showAnimations = checked }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Default Zoom Level"; color: theme.textPrimary; Layout.fillWidth: true }
                        SpinBox {
                            from: 50; to: 300; stepSize: 10
                            value: generalSettings.defaultZoom
                            onValueChanged: generalSettings.defaultZoom = value
                        }
                    }
                }
            }
        }
    }

    component SectionHeader : Text {
        property string label: ""
        text: label
        color: theme.accent
        font.pixelSize: 11
        font.bold: true
        font.letterSpacing: 0.8
        Layout.topMargin: 10
        Layout.bottomMargin: 4
    }

    component SettingsSwitch : RowLayout {
        property var theme
        property string label: ""
        property bool checked: false
        signal toggled(bool checked)
        Layout.fillWidth: true; height: 36
        Text { text: label; color: theme.textPrimary; font.pixelSize: 13; Layout.fillWidth: true }
        Switch { checked: parent.checked; onToggled: parent.toggled(checked) }
    }
}
