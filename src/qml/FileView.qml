import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Liphis.Core 1.0

Item {
    id: root
    focus: true
    activeFocusOnTab: true
    
    property string initialPath: ""
    property alias controller: controller
    property var theme: null
    property var itemMenuHandler: null

    signal requestedActive()
    signal tabTitleChanged(string title)

    property int keyboardIndex: -1
    property int rangeAnchorIndex: -1
    property string typeBuffer: ""
    property int cycleMatchIndex: -1

    function focusFileArea() {
        root.forceActiveFocus()
    }

    function openCreateFolderDialog() {
        createFolderDialog.open()
    }

    function currentRowCount() {
        return controller && controller.fileModel ? controller.fileModel.rowCount() : 0
    }

    function ensureKeyboardIndex() {
        var count = currentRowCount()
        if (count <= 0) {
            keyboardIndex = -1
            return
        }
        if (keyboardIndex < 0 || keyboardIndex >= count) {
            var selected = controller ? controller.selectedPath : ""
            var idx = selected ? controller.indexOfPath(selected) : -1
            keyboardIndex = idx >= 0 ? idx : 0
        }
    }

    function ensureVisible(index) {
        if (index < 0) return
        if (controller.viewMode === "grid") gv.positionViewAtIndex(index, GridView.Contain)
        else lv.positionViewAtIndex(index, ListView.Contain)
    }

    function applySelection(index, modifiers) {
        var count = currentRowCount()
        if (index < 0 || index >= count) return
        var prevIndex = keyboardIndex
        keyboardIndex = index
        var path = controller.pathAtIndex(index)
        if (!path || path.length === 0) return
        if (modifiers & Qt.ShiftModifier) {
            if (rangeAnchorIndex < 0) rangeAnchorIndex = prevIndex >= 0 ? prevIndex : keyboardIndex
            controller.selectRangeByIndexes(rangeAnchorIndex, keyboardIndex)
        } else if (modifiers & Qt.ControlModifier) {
            controller.toggleSelection(path)
            rangeAnchorIndex = keyboardIndex
        } else {
            controller.selectPath(path)
            rangeAnchorIndex = keyboardIndex
        }
        ensureVisible(index)
    }

    function moveCursor(step, modifiers) {
        var count = currentRowCount()
        if (count <= 0) return
        ensureKeyboardIndex()
        var next = keyboardIndex + step
        if (next < 0) next = 0
        if (next >= count) next = count - 1
        applySelection(next, modifiers)
    }

    function gridStepForUpDown() {
        var cols = Math.floor(gv.width / gv.cellWidth)
        return Math.max(1, cols)
    }

    function openCurrentSelection() {
        var path = controller.selectedPath
        if (!path || path.length === 0) {
            ensureKeyboardIndex()
            path = controller.pathAtIndex(keyboardIndex)
        }
        if (path && path.length > 0) controller.openPath(path)
    }

    function selectByTyping(charText) {
        var ch = charText.toLowerCase()
        if (!ch || ch.length !== 1) return
        var count = currentRowCount()
        if (count <= 0) return
        if (typeBuffer.length === 1 && typeBuffer === ch) {
            // Cycle among same-prefix matches on repeated key.
            var start = cycleMatchIndex >= 0 ? (cycleMatchIndex + 1) : 0
            for (var i = 0; i < count; ++i) {
                var idx = (start + i) % count
                var name = controller.nameAtIndex(idx).toLowerCase()
                if (name.startsWith(ch)) {
                    cycleMatchIndex = idx
                    applySelection(idx, 0)
                    typeResetTimer.restart()
                    return
                }
            }
            return
        }

        typeBuffer += ch
        cycleMatchIndex = -1
        for (var j = 0; j < count; ++j) {
            var name2 = controller.nameAtIndex(j).toLowerCase()
            if (name2.startsWith(typeBuffer)) {
                applySelection(j, 0)
                typeResetTimer.restart()
                return
            }
        }
        typeResetTimer.restart()
    }

    function displayName(name) {
        return String(name || "")
    }

    function requestVisibleThumbnails() {
        if (!controller || !controller.fileModel) return
        var count = currentRowCount()
        if (count <= 0) return

        var first = 0
        var last = Math.min(count - 1, 30)

        if (controller.viewMode === "grid") {
            first = gv.indexAt(6, gv.contentY + 6)
            if (first < 0) first = 0
            last = gv.indexAt(Math.max(6, gv.width - 10), gv.contentY + gv.height - 10)
            if (last < first) {
                var cols = Math.max(1, Math.floor(gv.width / gv.cellWidth))
                last = Math.min(count - 1, first + (cols * 5))
            }
        } else {
            first = lv.indexAt(6, lv.contentY + 6)
            if (first < 0) first = 0
            last = lv.indexAt(6, lv.contentY + lv.height - 10)
            if (last < first) last = Math.min(count - 1, first + 28)
        }

        controller.requestThumbnailsForRange(first, last)
    }

    Timer {
        id: typeResetTimer
        interval: 900
        repeat: false
        onTriggered: {
            root.typeBuffer = ""
            root.cycleMatchIndex = -1
        }
    }

    Timer {
        id: thumbsDebounce
        interval: 140
        repeat: false
        onTriggered: root.requestVisibleThumbnails()
    }

    Timer {
        id: thumbsDuringSearch
        interval: 260
        repeat: true
        running: !!(controller && controller.searchInProgress)
        onTriggered: root.requestVisibleThumbnails()
    }

    Keys.onPressed: (event) => {
        if (!controller) return
        if (event.modifiers === Qt.ControlModifier && event.key === Qt.Key_A) {
            controller.selectAll()
            event.accepted = true
            return
        }
        if (event.modifiers === (Qt.ControlModifier | Qt.ShiftModifier) && event.key === Qt.Key_N) {
            createFolderDialog.open()
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Delete) {
            if (controller.selectedPaths.length > 0) controller.trashItems(controller.selectedPaths)
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_F2) {
            if (controller.selectedPath && controller.selectedPath.length > 0) controller.startRename(controller.selectedPath)
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            openCurrentSelection()
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Backspace) {
            controller.goUp()
            event.accepted = true
            return
        }

        if (event.key === Qt.Key_Up) {
            moveCursor(controller.viewMode === "grid" ? -gridStepForUpDown() : -1, event.modifiers)
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Down) {
            moveCursor(controller.viewMode === "grid" ? gridStepForUpDown() : 1, event.modifiers)
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Left) {
            moveCursor(-1, event.modifiers)
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Right) {
            moveCursor(1, event.modifiers)
            event.accepted = true
            return
        }

        var t = event.text ? event.text.trim() : ""
        if (t.length === 1 && t.match(/[a-zA-Z0-9._-]/)) {
            selectByTyping(t)
            event.accepted = true
        }
    }

    function normalizePath(path) {
        if (!path) return ""
        var p = String(path)
        if (p.indexOf("file:///") === 0) {
            p = decodeURIComponent(p.substring("file://".length))
        } else if (p.indexOf("file://") === 0) {
            p = decodeURIComponent(p.substring("file://".length))
        }
        return p
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bg
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: root.activeFocus ? 1 : 0
        border.color: root.activeFocus ? theme.accentSoft : "transparent"
        radius: theme ? theme.rSm : 6
        z: 10
    }

    AppController {
        id: controller
        Component.onCompleted: {
            if (typeof globalThumbnailManager !== "undefined") setThumbnailManager(globalThumbnailManager);
            if (typeof globalPlacesModel !== "undefined") setPlacesModel(globalPlacesModel);
            var launchPath = initialPath && initialPath.length > 0 ? initialPath : homePath;
            launchPath = normalizePath(launchPath);
            if (launchPath && launchPath.length > 0) openPath(launchPath);
            root.tabTitleChanged(controller.title)
            root.requestedActive()
            root.forceActiveFocus()
        }
        onRenameRequested: (path) => renamingPath = path
        onCurrentPathChanged: {
            // Memory Optimization: Force garbage collection on major navigation
            gc();
            root.tabTitleChanged(controller.title)
        }
    }
    
    Component.onCompleted: {
        if (typeof updateActiveController === "function") updateActiveController()
    }

    readonly property string homePath: controller ? controller.homePath : ""

    readonly property color colTextPrimary: theme.textPrimary
    readonly property color colTextSecondary: theme.textSecondary
    property string renamingPath: ""

    Dialog {
        id: createFolderDialog
        title: "Create Folder"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 340
        anchors.centerIn: parent

        property alias folderName: folderNameField.text

        contentItem: ColumnLayout {
            spacing: 10
            Text { text: "Folder name"; color: root.colTextSecondary; font.pixelSize: 12 }
            TextField {
                id: folderNameField
                Layout.fillWidth: true
                placeholderText: "New Folder"
                selectByMouse: true
            }
        }

        onOpened: {
            folderNameField.text = ""
            folderNameField.forceActiveFocus()
        }
        onAccepted: {
            if (folderNameField.text.trim().length > 0) controller.createFolder(folderNameField.text.trim())
        }
    }

    // Context Menu for Background
    Menu {
        id: bgMenu
        Material.theme: (theme && theme.isDark) ? Material.Dark : Material.Light
        Material.background: theme.surfaceElevated
        MenuItem { text: "New Folder"; onTriggered: createFolderDialog.open() }
        MenuItem { text: "Show Hidden"; checkable: true; checked: controller.showHiddenFiles; onTriggered: controller.showHiddenFiles = checked }
        MenuItem { text: "Paste"; enabled: controller.hasClipboard; onTriggered: controller.pasteItem() }
        MenuItem { text: "Refresh"; onTriggered: controller.refresh() }
    }

    // --- DELEGATES ---

    Component {
        id: listDelegate
        Item {
            id: listRoot
            width: lv.width; height: 44 // A bit more breathing room
            readonly property bool isActuallySelected: controller.selectedPaths.indexOf(model.path) !== -1

            Rectangle {
                anchors.fill: parent; anchors.margins: 2; radius: theme ? theme.radiusSmall : 6
                color: listRoot.isActuallySelected ? theme.selection : (listMA.containsMouse ? theme.hover : "transparent")
                visible: listRoot.isActuallySelected || listMA.containsMouse
            }

            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 16
                FileIcon { Layout.preferredWidth: 22; Layout.preferredHeight: 22; isDir: model.isDir; iconName: model.iconName; theme: root.theme }
                Text {
                    text: root.displayName(model.name)
                    textFormat: Text.PlainText
                    Layout.fillWidth: true
                    Layout.preferredHeight: 18
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    clip: true
                    color: root.colTextPrimary
                    font.pixelSize: 13
                    font.weight: listRoot.isActuallySelected ? Font.Medium : Font.Normal
                }
                Text { text: model.formattedSize; Layout.preferredWidth: 80; horizontalAlignment: Text.AlignRight; color: root.colTextSecondary; font.pixelSize: 12 }
                Text { text: model.formattedDate; Layout.preferredWidth: 140; horizontalAlignment: Text.AlignRight; color: root.colTextSecondary; font.pixelSize: 12 }
            }

            MouseArea {
                id: listMA; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: (mouse) => {
                    var idx = controller.indexOfPath(model.path)
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        if (root.rangeAnchorIndex < 0) root.rangeAnchorIndex = idx
                        controller.selectRangeByIndexes(root.rangeAnchorIndex, idx)
                    } else if (mouse.modifiers & Qt.ControlModifier) controller.toggleSelection(model.path)
                    else controller.selectPath(model.path)
                    root.keyboardIndex = controller.indexOfPath(model.path)
                    if (!(mouse.modifiers & Qt.ShiftModifier)) root.rangeAnchorIndex = root.keyboardIndex
                    root.forceActiveFocus()
                    if (mouse.button === Qt.RightButton) {
                        if (typeof itemMenuHandler === "function") itemMenuHandler(model, controller)
                    }
                }
                onDoubleClicked: controller.openPath(model.path)
            }
        }
    }

    Component {
        id: gridDelegate
        Item {
            id: gridRoot
            width: gv.cellWidth; height: gv.cellHeight
            readonly property bool isActuallySelected: controller.selectedPaths.indexOf(model.path) !== -1

            Rectangle {
                anchors.fill: parent; anchors.margins: 4; radius: theme ? theme.radius : 8
                color: gridRoot.isActuallySelected ? theme.selection : (gridMA.containsMouse ? theme.hover : "transparent")
                visible: gridRoot.isActuallySelected || gridMA.containsMouse
            }

            ColumnLayout {
                anchors.fill: parent; anchors.margins: 12; spacing: 8
                Item {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    FileIcon { anchors.centerIn: parent; width: 56; height: 56; isDir: model.isDir; iconName: model.iconName; theme: root.theme; visible: !gridThumb.visible }
                    Image { 
                        id: gridThumb; anchors.fill: parent; fillMode: Image.PreserveAspectFit; source: model.thumbnail ? model.thumbnail : ""
                        visible: status === Image.Ready; asynchronous: false; cache: true // Sync for memory predictability
                    }
                }
                Text { 
                    text: root.displayName(model.name)
                    textFormat: Text.PlainText
                    Layout.fillWidth: true
                    Layout.preferredHeight: gridRoot.isActuallySelected ? 30 : 16
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    wrapMode: gridRoot.isActuallySelected ? Text.WrapAnywhere : Text.NoWrap
                    maximumLineCount: gridRoot.isActuallySelected ? 2 : 1
                    clip: true
                    color: root.colTextPrimary; font.pixelSize: 12; font.weight: gridRoot.isActuallySelected ? Font.Medium : Font.Normal
                }
            }

            MouseArea {
                id: gridMA; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: (mouse) => {
                    var idx = controller.indexOfPath(model.path)
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        if (root.rangeAnchorIndex < 0) root.rangeAnchorIndex = idx
                        controller.selectRangeByIndexes(root.rangeAnchorIndex, idx)
                    } else if (mouse.modifiers & Qt.ControlModifier) controller.toggleSelection(model.path)
                    else controller.selectPath(model.path)
                    root.keyboardIndex = controller.indexOfPath(model.path)
                    if (!(mouse.modifiers & Qt.ShiftModifier)) root.rangeAnchorIndex = root.keyboardIndex
                    root.forceActiveFocus()
                    if (mouse.button === Qt.RightButton) {
                        if (typeof itemMenuHandler === "function") itemMenuHandler(model, controller)
                    }
                }
                onDoubleClicked: controller.openPath(model.path)
            }
        }
    }

    // --- MAIN VIEW ---

    StackLayout {
        anchors.fill: parent
        currentIndex: controller.viewMode === "grid" ? 1 : 0

        ListView {
            id: lv; model: controller.fileModel; clip: true; delegate: listDelegate
            cacheBuffer: 0 // Absolute minimum memory
            onContentYChanged: thumbsDebounce.restart()
            onCountChanged: thumbsDebounce.restart()
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }

        GridView {
            id: gv; model: controller.fileModel; clip: true; delegate: gridDelegate
            cellWidth: 100; cellHeight: 100; cacheBuffer: 0
            onContentYChanged: thumbsDebounce.restart()
            onCountChanged: thumbsDebounce.restart()
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
        }
    }

    Connections {
        target: controller
        function onViewModeChanged() { thumbsDebounce.restart() }
        function onCurrentPathChanged() { thumbsDebounce.restart() }
    }

    MouseArea {
        anchors.fill: parent; z: -1; acceptedButtons: Qt.RightButton | Qt.BackButton | Qt.ForwardButton | Qt.LeftButton
        onClicked: (mouse) => {
            root.forceActiveFocus()
            if (mouse.button === Qt.RightButton) bgMenu.popup()
            else if (mouse.button === Qt.BackButton) controller.goBack()
            else if (mouse.button === Qt.ForwardButton) controller.goForward()
            else if (mouse.button === Qt.LeftButton) controller.clearSelection()
        }
    }
}
