import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var activeController: null
    property bool detailsVisible: true
    property bool terminalVisible: false
    property bool manualPathEntry: false

    signal detailsToggleRequested()
    signal terminalToggleRequested()
    signal settingsRequested()

    height: theme.headerHeight
    color: theme.header

    property bool themeAnimating: false
    property string brandScrambleText: ""
    readonly property string brandStableText: "L I P H I S"
    readonly property bool narrow: width < 1260
    readonly property bool veryNarrow: width < 1080
    readonly property bool compactSearchControls: width < 1460

    function focusSearchField() {
        searchField.forceActiveFocus()
        searchField.selectAll()
    }

    function focusPathField() {
        manualPathEntry = true
        pathField.forceActiveFocus()
        pathField.selectAll()
    }

    function applySearch(immediateSave) {
        if (!root.activeController) return
        var raw = searchField.text ? searchField.text.trim() : ""
        if (root.activeController.searchMode === "global" && !immediateSave) {
            // Avoid heavy global scans for accidental short input.
            if (raw.length === 0) return
            if (raw.length < 2) return
        }
        var q = raw
        if (typePreset.currentValue && typePreset.currentValue !== "all")
            q = (q.length > 0 ? q + " " : "") + "kind:" + typePreset.currentValue
        root.activeController.applySearchQuery(q)
        if (immediateSave) root.activeController.saveSearchQuery(q)
    }

    function clearSearchAndRestore() {
        if (!root.activeController) return
        searchDebounce.stop()
        searchField.text = ""
        if (typePreset) typePreset.currentIndex = 0
        if (root.activeController.searchMode === "global") {
            root.activeController.cancelSearch()
            root.activeController.searchMode = "local"
            root.activeController.searchScope = "current"
        }
        root.activeController.applySearchQuery("")
    }

    function hasActiveSearch() {
        if (!root.activeController) return false
        if (searchField && searchField.text && searchField.text.trim().length > 0) return true
        return root.activeController.searchMode === "global" && root.activeController.searchInProgress
    }

    function scrambledBrandText() {
        var chars = ["L", "I", "P", "H", "I", "S"]
        for (var i = chars.length - 1; i > 0; --i) {
            var j = Math.floor(Math.random() * (i + 1))
            var t = chars[i]
            chars[i] = chars[j]
            chars[j] = t
        }
        return chars.join(" ")
    }

    function toggleThemeWithAnimation() {
        if (themeAnimating) return
        themeAnimating = true
        brandScrambleText = scrambledBrandText()
        themeSwapAnimation.start()
    }

    Timer {
        id: brandShuffleTimer
        interval: 60
        repeat: true
        running: root.themeAnimating
        onTriggered: root.brandScrambleText = root.scrambledBrandText()
    }

    Timer {
        id: searchDebounce
        interval: root.activeController && root.activeController.searchMode === "global" ? 650 : 220
        repeat: false
        onTriggered: root.applySearch(false)
    }

    Connections {
        target: root.activeController
        function onRequestSearchClear() {
            searchField.text = ""
            if (typePreset) typePreset.currentIndex = 0
        }
    }

    SequentialAnimation {
        id: themeSwapAnimation
        PropertyAnimation {
            target: logoBadge
            property: "scale"
            from: 1.0
            to: 1.12
            duration: 160
            easing.type: Easing.OutCubic
        }
        PauseAnimation { duration: 80 }
        ScriptAction { script: root.theme.isDark = !root.theme.isDark }
        ParallelAnimation {
            NumberAnimation {
                target: logoBadge
                property: "rotation"
                from: 0
                to: 360
                duration: 380
                easing.type: Easing.InOutCubic
            }
            PropertyAnimation {
                target: logoBadge
                property: "scale"
                from: 1.12
                to: 1.0
                duration: 380
                easing.type: Easing.OutBack
            }
        }
        ScriptAction {
            script: {
                root.themeAnimating = false
                root.brandScrambleText = ""
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: theme ? (root.narrow ? theme.space12 : theme.space16) : 12
        anchors.rightMargin: theme ? (root.narrow ? theme.space12 : theme.space16) : 12
        spacing: theme ? (root.narrow ? theme.space8 : theme.space12) : 8

        ToolButton {
            id: logoButton
            Layout.preferredHeight: theme ? theme.controlMd : 40
            Layout.preferredWidth: root.veryNarrow ? 142 : 196
            padding: 0
            onClicked: root.toggleThemeWithAnimation()
            ToolTip.visible: hovered
            ToolTip.text: "Toggle theme"
            background: Rectangle {
                radius: root.theme.rSm
                color: logoButton.hovered ? root.theme.hover : "transparent"
                border.color: logoButton.hovered ? root.theme.border : "transparent"
                border.width: 1
            }
            contentItem: RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.theme ? root.theme.space8 + 2 : 10
                anchors.rightMargin: root.theme ? root.theme.space8 + 2 : 10
                spacing: root.theme ? root.theme.space8 : 8

                Image {
                    id: logoBadge
                    source: root.theme && root.theme.isDark ? "assets/logo-dark.png" : "assets/logo-light.png"
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: 120
                    sourceSize.height: 120
                    smooth: true
                    antialiasing: true
                    mipmap: true
                    opacity: 0.96
                    transformOrigin: Item.Center
                }

                Text {
                    text: root.themeAnimating ? root.brandScrambleText : root.brandStableText
                    visible: !root.veryNarrow
                    color: root.theme.textPrimary
                    font.pixelSize: 15
                    font.bold: true
                    font.letterSpacing: root.theme ? root.theme.letterSpacingBrand : 1.8
                    opacity: 0.95
                }
            }
        }

        RowLayout {
            spacing: theme ? theme.space4 : 4

            ThemedIconButton {
                theme: root.theme
                iconName: "back"
                toolTip: "Back"
                enabled: root.activeController && root.activeController.canGoBack
                onClicked: root.activeController.goBack()
            }
            ThemedIconButton {
                theme: root.theme
                iconName: "forward"
                toolTip: "Forward"
                enabled: root.activeController && root.activeController.canGoForward
                onClicked: root.activeController.goForward()
            }
            ThemedIconButton {
                theme: root.theme
                iconName: "up"
                toolTip: "Up"
                enabled: !!root.activeController
                onClicked: root.activeController.goUp()
            }
            ThemedIconButton {
                theme: root.theme
                iconName: "refresh"
                toolTip: "Refresh"
                enabled: !!root.activeController
                onClicked: root.activeController.refresh()
            }
        }

        Rectangle {
            id: pathContainer
            Layout.fillWidth: true
            Layout.minimumWidth: root.veryNarrow ? 250 : 320
            Layout.preferredHeight: theme.controlMd
            radius: theme.rMd
            color: theme.surfaceRaised
            border.color: theme.border
            border.width: 1

            StackLayout {
                anchors.fill: parent
                anchors.leftMargin: theme ? theme.space12 : 12
                anchors.rightMargin: theme ? theme.space6 : 6
                currentIndex: root.manualPathEntry ? 1 : 0

                Breadcrumbs {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    path: root.activeController ? root.activeController.currentPath : ""
                    theme: root.theme
                    onSegmentClicked: (newPath) => { if (root.activeController) root.activeController.openPath(newPath) }
                    onManualEntryRequested: root.focusPathField()
                }

                RowLayout {
                    spacing: theme ? theme.space6 : 6
                    Item {
                        Layout.preferredWidth: theme.iconSm + 4
                        Layout.fillHeight: true
                        Icon {
                            anchors.centerIn: parent
                            name: "folder"
                            iconSize: theme.iconSm
                            color: theme.textSecondary
                        }
                    }

                    TextField {
                        id: pathField
                        Layout.fillWidth: true
                        color: theme.textPrimary
                        font.pixelSize: theme ? theme.fontBody : 12
                        padding: 0
                        background: null
                        text: root.activeController ? root.activeController.currentPath : ""
                        selectByMouse: true
                        selectionColor: theme.selection
                        onAccepted: {
                            if (root.activeController) root.activeController.openPath(text)
                            root.manualPathEntry = false
                        }
                        onActiveFocusChanged: if (!activeFocus) root.manualPathEntry = false
                    }
                }
            }
            
            ThemedIconButton {
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                theme: root.theme
                implicitWidth: root.theme.controlSm
                implicitHeight: root.theme.controlSm
                readonly property bool bookmarked: !!(root.activeController
                    && root.activeController.currentPath
                    && root.activeController.bookmarksRevision >= 0
                    && root.activeController.isBookmarked(root.activeController.currentPath))
                iconName: bookmarked ? "star-filled" : "star"
                iconSize: 15
                toolTip: bookmarked ? "Remove from Bookmark" : "Add to Bookmark"
                enabled: !!root.activeController
                onClicked: root.activeController.toggleBookmark(root.activeController.currentPath, root.activeController.title)
            }
        }

        Rectangle {
            id: searchContainer
            Layout.preferredWidth: {
                let base = root.veryNarrow ? 310 : (root.narrow ? 360 : 430)
                if (searchField.activeFocus || (searchField.text && searchField.text.trim().length > 0)) {
                    return root.veryNarrow ? 400 : (root.narrow ? 500 : 620)
                }
                return base
            }
            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
            }
            Layout.preferredHeight: theme.controlMd
            radius: theme.rMd
            color: theme.surfaceRaised
            border.color: theme.border
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: theme ? theme.space8 : 8
                anchors.rightMargin: theme ? theme.space8 : 8
                spacing: theme ? theme.space6 : 6

                Rectangle {
                    Layout.preferredWidth: 160
                    Layout.preferredHeight: theme.controlSm
                    radius: theme.rSm
                    color: theme.surface
                    border.color: theme.border
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 2
                        spacing: 2

                        ToolButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: "Local"
                            font.pixelSize: theme.fontLabel
                            onClicked: {
                                if (!root.activeController) return
                                root.activeController.searchMode = "local"
                                root.applySearch(false)
                            }
                            background: Rectangle {
                                radius: theme.rSm
                                color: root.activeController && root.activeController.searchMode === "local"
                                    ? theme.selection
                                    : "transparent"
                            }
                        }
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: "Global"
                            font.pixelSize: theme.fontLabel
                            onClicked: {
                                if (!root.activeController) return
                                root.activeController.searchMode = "global"
                                root.activeController.searchContent = false
                                root.applySearch(false)
                            }
                            background: Rectangle {
                                radius: theme.rSm
                                color: root.activeController && root.activeController.searchMode === "global" && !root.activeController.searchContent
                                    ? theme.selection
                                    : "transparent"
                            }
                        }
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: "Content"
                            font.pixelSize: theme.fontLabel
                            onClicked: {
                                if (!root.activeController) return
                                root.activeController.searchMode = "global"
                                root.activeController.searchContent = true
                                root.applySearch(false)
                            }
                            background: Rectangle {
                                radius: theme.rSm
                                color: root.activeController && root.activeController.searchContent
                                    ? theme.selection
                                    : "transparent"
                            }
                        }
                    }
                }

                ComboBox {
                    id: typePreset
                    Layout.preferredWidth: 86
                    Layout.preferredHeight: theme.controlSm
                    model: [
                        { label: "All", value: "all" },
                        { label: "Images", value: "image" },
                        { label: "Video", value: "video" },
                        { label: "Audio", value: "audio" },
                        { label: "Docs", value: "document" },
                        { label: "Code", value: "code" },
                        { label: "Archive", value: "archive" },
                        { label: "Folders", value: "folder" }
                    ]
                    textRole: "label"
                    valueRole: "value"
                    onActivated: root.applySearch(false)
                    visible: !root.compactSearchControls
                    background: Rectangle {
                        radius: theme.rSm
                        color: typePreset.hovered ? theme.hover : theme.surface
                        border.color: theme.border
                        border.width: 1
                    }
                    contentItem: Text {
                        leftPadding: 10
                        rightPadding: 20
                        text: typePreset.displayText
                        color: theme.textPrimary
                        font.pixelSize: theme.fontBody - 1
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                    indicator: Text {
                        text: "▾"
                        color: theme.textSecondary
                        font.pixelSize: 10
                        anchors.right: parent.right
                        anchors.rightMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Item {
                    Layout.preferredWidth: theme.iconSm + 2
                    Layout.fillHeight: true
                    Icon {
                        anchors.centerIn: parent
                        name: "search"
                        iconSize: theme.iconSm
                        color: theme.textSecondary
                    }
                }

                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: root.veryNarrow
                        ? "Search... ext:pdf"
                        : "Search...  ext:pdf size:>10MB modified:7d"
                    color: theme.textPrimary
                    placeholderTextColor: theme.textSecondary
                    selectedTextColor: theme.surface
                    selectionColor: theme.accentSoft
                    cursorVisible: activeFocus
                    font.pixelSize: theme ? theme.fontBody : 12
                    leftPadding: 6
                    rightPadding: 6
                    background: null
                    onTextChanged: searchDebounce.restart()
                    onAccepted: {
                        searchDebounce.stop()
                        root.applySearch(true)
                    }
                    Keys.onEscapePressed: (event) => {
                        if (searchField.text !== "") {
                            root.clearSearchAndRestore()
                            event.accepted = true
                        } else {
                            event.accepted = false // Propagate to global shortcut
                        }
                    }
                }

                ToolButton {
                    id: compactFiltersButton
                    visible: root.compactSearchControls
                    Layout.preferredWidth: Math.max(theme.controlSm + 8, 52)
                    Layout.preferredHeight: theme.controlSm
                    text: typePreset.displayText
                    font.pixelSize: theme.fontLabel
                    onClicked: compactFiltersPopup.open()
                    background: Rectangle {
                        radius: theme.rSm
                        color: compactFiltersButton.hovered ? theme.hover : theme.surface
                        border.color: theme.border
                        border.width: 1
                    }
                    contentItem: Text {
                        text: compactFiltersButton.text
                        color: theme.textPrimary
                        font.pixelSize: theme.fontLabel
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                    ToolTip.visible: hovered
                    ToolTip.text: "Search filters"
                }

                ToolButton {
                    id: recentButton
                    Layout.preferredWidth: theme.controlSm
                    Layout.preferredHeight: theme.controlSm
                    visible: root.activeController && root.activeController.recentSearches
                        && root.activeController.recentSearches.length > 0
                    text: "▾"
                    font.pixelSize: 11
                    onClicked: recentPopup.open()
                    background: Rectangle {
                        radius: theme.rSm
                        color: recentButton.hovered ? theme.hover : "transparent"
                    }
                }

                ToolButton {
                    id: cancelSearchButton
                    visible: !!(root.activeController && root.activeController.searchInProgress)
                    Layout.preferredWidth: theme.controlSm
                    Layout.preferredHeight: theme.controlSm
                    text: "✕"
                    font.pixelSize: 12
                    onClicked: if (root.activeController) root.activeController.cancelSearch()
                    background: Rectangle {
                        radius: theme.rSm
                        color: cancelSearchButton.hovered ? theme.hover : "transparent"
                    }
                }
            }
        }

        Popup {
            id: recentPopup
            readonly property point buttonPos: recentButton.mapToItem(root, 0, 0)
            y: buttonPos.y + recentButton.height + 4
            x: Math.max(theme ? theme.space16 : 16, buttonPos.x + recentButton.width - width)
            width: 320
            padding: 6
            modal: false
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            background: Rectangle {
                radius: theme.rSm
                color: theme.surfaceRaised
                border.color: theme.border
                border.width: 1
            }
            contentItem: Column {
                spacing: 2
                Repeater {
                    model: root.activeController ? root.activeController.recentSearches : []
                    delegate: ToolButton {
                        width: parent.width
                        text: modelData
                        horizontalPadding: 10
                        verticalPadding: 8
                        font.pixelSize: theme.fontBody
                        onClicked: {
                            searchField.text = modelData
                            root.applySearch(true)
                            recentPopup.close()
                        }
                        background: Rectangle {
                            radius: theme.rSm
                            color: parent.hovered ? theme.hover : "transparent"
                        }
                    }
                }
            }
        }

        Popup {
            id: compactFiltersPopup
            readonly property point buttonPos: compactFiltersButton.mapToItem(root, 0, 0)
            y: buttonPos.y + compactFiltersButton.height + 4
            x: Math.max(theme ? theme.space16 : 16, buttonPos.x + compactFiltersButton.width - width)
            width: 240
            padding: 8
            modal: false
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            background: Rectangle {
                radius: theme.rSm
                color: theme.surfaceRaised
                border.color: theme.border
                border.width: 1
            }
            contentItem: ColumnLayout {
                spacing: 8

                Text {
                    text: "Search Filters"
                    color: theme.textSecondary
                    font.pixelSize: theme.fontLabel
                    font.letterSpacing: 0.6
                }

                ComboBox {
                    Layout.fillWidth: true
                    model: typePreset.model
                    textRole: "label"
                    valueRole: "value"
                    currentIndex: typePreset.currentIndex
                    onActivated: {
                        typePreset.currentIndex = currentIndex
                        root.applySearch(false)
                    }
                }
            }
        }

        RowLayout {
            spacing: theme ? theme.space4 : 4

            ThemedIconButton {
                theme: root.theme
                iconName: "info"
                toolTip: "Toggle Inspector"
                checkable: true
                checked: root.detailsVisible
                accentWhenChecked: true
                onClicked: root.detailsToggleRequested()
            }
            ThemedIconButton {
                theme: root.theme
                iconName: "terminal"
                toolTip: "Toggle Terminal"
                checkable: true
                checked: root.terminalVisible
                accentWhenChecked: true
                onClicked: root.terminalToggleRequested()
            }
            ThemedIconButton {
                theme: root.theme
                iconName: "settings"
                toolTip: "Settings"
                onClicked: root.settingsRequested()
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
