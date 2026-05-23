import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtCore

Item {
    id: root

    property var theme: null
    property var activeController: null
    property var placesModel: null
    property var docIntelController: null
    property string activePath: ""
    property string homePath: ""

    readonly property bool expanded: (typeof generalSettings !== "undefined" && generalSettings) ? generalSettings.sidebarExpanded : true
    readonly property bool compact: !expanded

    property var networkEntries: []
    property var deviceEntries: []
    property var bookmarkEntries: []

    signal toggleExpanded()
    signal pathActivated(string path)
    signal removeBookmarkRequested(string path)
    signal openDocInboxRequested()
    signal openDocDuplicatesRequested()
    signal openDocHealthRequested()

    function value(key, fallback) {
        return (theme && theme[key] !== undefined && theme[key] !== null) ? theme[key] : fallback
    }

    function settingValue(key, fallback) {
        return (typeof generalSettings !== "undefined" && generalSettings && generalSettings[key] !== undefined)
                ? generalSettings[key]
                : fallback
    }

    function storageRatio(usedBytes, totalBytes) {
        var total = Number(totalBytes || 0)
        if (total <= 0)
            return 0
        return Math.max(0, Math.min(1, Number(usedBytes || 0) / total))
    }

    function formatGiB(bytes) {
        var b = Number(bytes || 0)
        if (b <= 0)
            return ""
        return (b / (1024.0 * 1024.0 * 1024.0)).toFixed(1) + " GiB"
    }

    function shortName(path) {
        var parts = String(path || "").split("/")
        return parts.length ? parts[parts.length - 1] : String(path || "")
    }

    function refreshSidebarLists() {
        if (!placesModel || typeof placesModel.entriesByCategory !== "function") {
            networkEntries = []
            deviceEntries = []
            bookmarkEntries = []
            return
        }

        networkEntries = placesModel.entriesByCategory(4)
        deviceEntries = placesModel.entriesByCategory(2)
        bookmarkEntries = placesModel.entriesByCategory(1)
    }

    width: root.expanded ? root.value("sidebarWidth", 220) : 64

    Behavior on width {
        enabled: root.settingValue("showAnimations", true)
        NumberAnimation {
            duration: 170
            easing.type: Easing.OutQuad
        }
    }

    Rectangle {
        anchors.fill: parent
        color: root.value("surfaceMuted", "#1f232a")
    }

    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: root.value("border", "#ffffff")
        opacity: 0.18
    }

    Menu {
        id: sidebarBgMenu
        Material.theme: root.value("isDark", true) ? Material.Dark : Material.Light
        Material.background: root.value("surfaceElevated", "#2a2f37")

        MenuItem {
            text: "Show Places"
            checkable: true
            checked: root.settingValue("showPlaces", true)
            onTriggered: {
                if (typeof generalSettings !== "undefined" && generalSettings)
                    generalSettings.showPlaces = checked
            }
        }
        MenuItem {
            text: "Show Bookmarks"
            checkable: true
            checked: root.settingValue("showBookmarks", true)
            onTriggered: {
                if (typeof generalSettings !== "undefined" && generalSettings)
                    generalSettings.showBookmarks = checked
            }
        }
        MenuItem {
            text: "Show Devices"
            checkable: true
            checked: root.settingValue("showDevices", true)
            onTriggered: {
                if (typeof generalSettings !== "undefined" && generalSettings)
                    generalSettings.showDevices = checked
            }
        }
        MenuItem {
            text: "Show Recent"
            checkable: true
            checked: root.settingValue("showRecent", true)
            onTriggered: {
                if (typeof generalSettings !== "undefined" && generalSettings)
                    generalSettings.showRecent = checked
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        z: -1
        acceptedButtons: Qt.RightButton
        onClicked: function(mouse) {
            if (mouse.button === Qt.RightButton)
                sidebarBgMenu.popup()
        }
    }

    Component.onCompleted: refreshSidebarLists()
    onPlacesModelChanged: refreshSidebarLists()

    Connections {
        target: placesModel
        function onModelReset() { root.refreshSidebarLists() }
        function onRowsInserted() { root.refreshSidebarLists() }
        function onRowsRemoved() { root.refreshSidebarLists() }
        function onDataChanged() { root.refreshSidebarLists() }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.value("headerHeight", 52)
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                anchors.topMargin: 4
                anchors.bottomMargin: 4

                Item { Layout.fillWidth: true }

                ThemedIconButton {
                    theme: root.theme
                    iconName: root.expanded ? "layout-sidebar-left-collapse" : "layout-sidebar-left-expand"
                    iconSize: 18
                    toolTip: root.expanded ? "Collapse Sidebar" : "Expand Sidebar"
                    onClicked: root.toggleExpanded()
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 1
                color: root.value("border", "#ffffff")
                opacity: 0.14
            }
        }

        ScrollView {
            id: sidebarScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                width: sidebarScroll.availableWidth
                spacing: 12
                Layout.topMargin: 12
                Layout.bottomMargin: 16

                // Places
                ColumnLayout {
                    visible: root.settingValue("showPlaces", true)
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: "PLACES"
                        Layout.leftMargin: 16
                        visible: root.expanded
                        color: root.value("textTertiary", "#94a3b8")
                        font.pixelSize: 9
                        font.weight: Font.Normal
                        font.letterSpacing: 1.2
                    }

                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "") === String(root.homePath || "")
                        iconName: "home"
                        text: "Home"
                        expanded: root.expanded
                        onClicked: root.pathActivated(root.homePath)
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "").endsWith("/Documents")
                        iconName: "folder"
                        text: "Documents"
                        expanded: root.expanded
                        onClicked: root.pathActivated(root.homePath + "/Documents")
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "").endsWith("/Downloads")
                        iconName: "download"
                        text: "Downloads"
                        expanded: root.expanded
                        onClicked: root.pathActivated(root.homePath + "/Downloads")
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "").endsWith("/Pictures")
                        iconName: "photo"
                        text: "Pictures"
                        expanded: root.expanded
                        onClicked: root.pathActivated(root.homePath + "/Pictures")
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "").endsWith("/Videos")
                        iconName: "video"
                        text: "Videos"
                        expanded: root.expanded
                        onClicked: root.pathActivated(root.homePath + "/Videos")
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "").endsWith("/Music")
                        iconName: "music"
                        text: "Music"
                        expanded: root.expanded
                        onClicked: root.pathActivated(root.homePath + "/Music")
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        theme: root.theme
                        active: String(root.activePath || "") === "trash:///"
                        iconName: "trash"
                        text: "Trash"
                        expanded: root.expanded
                        onClicked: root.pathActivated("trash:///")
                    }

                    Repeater {
                        model: root.bookmarkEntries
                        SidebarItem {
                            Layout.fillWidth: true
                            theme: root.theme
                            active: root.activePath === modelData.path
                            iconName: modelData.icon || "folder-star"
                            text: modelData.name
                            expanded: root.expanded
                            onClicked: root.pathActivated(modelData.path)
                        }
                    }
                }

                // Network
                ColumnLayout {
                    visible: root.networkEntries && root.networkEntries.length > 0
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: "NETWORK"
                        Layout.leftMargin: 16
                        visible: root.expanded
                        color: root.value("textTertiary", "#94a3b8")
                        font.pixelSize: 9
                        font.weight: Font.Normal
                        font.letterSpacing: 1.2
                    }

                    Repeater {
                        model: root.networkEntries
                        SidebarItem {
                            Layout.fillWidth: true
                            theme: root.theme
                            active: root.activePath === modelData.path
                            iconName: modelData.icon || "network"
                            text: modelData.name
                            expanded: root.expanded
                            onClicked: root.pathActivated(modelData.path)
                        }
                    }
                }

                // Devices
                ColumnLayout {
                    visible: root.settingValue("showDevices", true) && root.deviceEntries && root.deviceEntries.length > 0
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: "DEVICES"
                        Layout.leftMargin: 16
                        visible: root.expanded
                        color: root.value("textTertiary", "#94a3b8")
                        font.pixelSize: 9
                        font.weight: Font.Normal
                        font.letterSpacing: 1.2
                    }

                    Repeater {
                        model: root.deviceEntries
                        delegate: Item {
                            Layout.fillWidth: true
                            Layout.preferredHeight: root.expanded ? 52 : 36

                            SidebarItem {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                height: root.expanded ? 28 : parent.height
                                theme: root.theme
                                active: root.activePath === modelData.path
                                iconName: modelData.icon || "drive-harddisk"
                                text: root.expanded ? modelData.name : ""
                                expanded: root.expanded
                                onClicked: root.pathActivated(modelData.path)
                                ToolTip.visible: !root.expanded && hovered
                                ToolTip.text: modelData.name + (modelData.totalBytes > 0 ? ("\n" + root.formatGiB(modelData.usedBytes) + " / " + root.formatGiB(modelData.totalBytes)) : "")
                            }

                            Item {
                                id: storageMeter
                                visible: root.expanded && modelData.totalBytes > 0
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                anchors.leftMargin: 16
                                anchors.rightMargin: 16
                                height: 16
                                clip: true

                                readonly property real ratio: root.storageRatio(modelData.usedBytes, modelData.totalBytes)

                                Text {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    text: root.formatGiB(modelData.usedBytes) + " / " + root.formatGiB(modelData.totalBytes)
                                    color: root.value("textTertiary", "#94a3b8")
                                    font.pixelSize: 9
                                }

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 2
                                    height: 4
                                    radius: 2
                                    color: root.value("surfaceRaised", "#334155")
                                }
                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 2
                                    width: Math.max(4, parent.width * storageMeter.ratio)
                                    height: 4
                                    radius: 2
                                    color: storageMeter.ratio > 0.9 ? root.value("error", "#ef4444") : root.value("accent", "#60a5fa")
                                }
                            }
                        }
                    }
                }

                // Recent
                ColumnLayout {
                    visible: root.settingValue("showRecent", true) && root.activeController
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: "RECENT"
                        Layout.leftMargin: 16
                        visible: root.expanded
                        color: root.value("textTertiary", "#94a3b8")
                        font.pixelSize: 9
                        font.weight: Font.Normal
                        font.letterSpacing: 1.2
                    }

                    Repeater {
                        model: root.activeController ? root.activeController.recentPaths : []
                        SidebarItem {
                            Layout.fillWidth: true
                            theme: root.theme
                            active: false
                            iconName: "folder"
                            text: root.expanded ? root.shortName(modelData) : ""
                            expanded: root.expanded
                            onClicked: root.pathActivated(modelData)
                            ToolTip.visible: hovered
                            ToolTip.text: modelData
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }

        // Doc Intel
        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: root.expanded ? 12 : 6
            Layout.rightMargin: root.expanded ? 12 : 6
            Layout.topMargin: 8
            Layout.bottomMargin: 10
            spacing: root.expanded ? 8 : 6

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: root.value("border", "#ffffff")
                opacity: 0.10
            }

            Text {
                text: "DOC INTEL"
                visible: root.expanded
                font.pixelSize: 9
                color: root.value("textTertiary", "#94a3b8")
                font.weight: Font.Normal
                font.letterSpacing: 1.2
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
            }

            GridLayout {
                Layout.fillWidth: true
                visible: root.expanded
                columns: 3
                columnSpacing: 8
                rowSpacing: 8

                Button {
                    id: inboxBtn
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Inbox"
                    onClicked: root.openDocInboxRequested()
                    background: Rectangle {
                        radius: 8
                        color: inboxBtn.hovered ? root.value("hover", "#334155") : root.value("surfaceRaised", "#2b313a")
                        border.color: root.value("border", "#ffffff")
                        border.width: 1
                    }
                    contentItem: RowLayout {
                        spacing: 6
                        Icon { name: "folder-open"; iconSize: 12; color: root.value("textSecondary", "#cbd5e1") }
                        Text {
                            text: inboxBtn.text
                            color: root.value("textPrimary", "#f8fafc")
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                Button {
                    id: dupesBtn
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Dupes"
                    onClicked: root.openDocDuplicatesRequested()
                    background: Rectangle {
                        radius: 8
                        color: dupesBtn.hovered ? root.value("hover", "#334155") : root.value("surfaceRaised", "#2b313a")
                        border.color: root.value("border", "#ffffff")
                        border.width: 1
                    }
                    contentItem: RowLayout {
                        spacing: 6
                        Icon { name: "copy"; iconSize: 12; color: root.value("textSecondary", "#cbd5e1") }
                        Text {
                            text: dupesBtn.text
                            color: root.value("textPrimary", "#f8fafc")
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                Button {
                    id: healthBtn
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    text: "Health"
                    onClicked: root.openDocHealthRequested()
                    background: Rectangle {
                        radius: 8
                        color: healthBtn.hovered ? root.value("hover", "#334155") : root.value("surfaceRaised", "#2b313a")
                        border.color: root.value("border", "#ffffff")
                        border.width: 1
                    }
                    contentItem: RowLayout {
                        spacing: 6
                        Icon { name: "chart-pie"; iconSize: 12; color: root.value("textSecondary", "#cbd5e1") }
                        Text {
                            text: healthBtn.text
                            color: root.value("textPrimary", "#f8fafc")
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8
                visible: root.compact
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 2

                ThemedIconButton {
                    Layout.alignment: Qt.AlignHCenter
                    theme: root.theme
                    iconName: "folder-open"
                    iconSize: 18
                    toolTip: "Doc Intel: Inbox"
                    onClicked: root.openDocInboxRequested()
                }
                ThemedIconButton {
                    Layout.alignment: Qt.AlignHCenter
                    theme: root.theme
                    iconName: "copy"
                    iconSize: 18
                    toolTip: "Doc Intel: Duplicates"
                    onClicked: root.openDocDuplicatesRequested()
                }
                ThemedIconButton {
                    Layout.alignment: Qt.AlignHCenter
                    theme: root.theme
                    iconName: "chart-pie"
                    iconSize: 18
                    toolTip: "Doc Intel: Health"
                    onClicked: root.openDocHealthRequested()
                }
            }
        }
    }
}
