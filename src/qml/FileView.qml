import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtCore
import Liphis.Core 1.0

Item {
    id: root
    focus: true
    activeFocusOnTab: true
    Keys.priority: Keys.BeforeItem

    DropArea {
        anchors.fill: parent
        onDropped: (drop) => {
            if (drop.hasText) {
                let paths = drop.text.split("\n")
                controller.dropItems(paths, controller.currentPath, (drop.supportedActions & Qt.CopyAction) && (drop.proposedAction === Qt.ProposedAction))
            }
        }
    }

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
    property real pendingTouchpadDeltaY: 0
    property var pendingTouchpadView: null

    property int zoomLevel: generalSettings ? generalSettings.defaultZoom : 100
    property int pendingZoomLevel: zoomLevel
    readonly property int baseCellSize: 108
    readonly property int baseIconSize: 86
    readonly property int baseListHeight: 46

    readonly property real zoomScale: zoomLevel / 100.0

    function queueZoom(nextZoom) {
        var quantized = Math.round(Math.max(50, Math.min(150, nextZoom)) / 5) * 5
        if (pendingZoomLevel === quantized) return
        pendingZoomLevel = quantized
        zoomApplyTimer.restart()
    }
    function zoomIn()    { queueZoom(pendingZoomLevel + 10) }
    function zoomOut()   { queueZoom(pendingZoomLevel - 10) }
    function resetZoom() { queueZoom(generalSettings ? generalSettings.defaultZoom : 100) }

    Timer {
        id: zoomApplyTimer
        interval: 80
        repeat: false
        onTriggered: {
            if (zoomLevel !== pendingZoomLevel) zoomLevel = pendingZoomLevel
        }
    }

    function focusFileArea() {
        root.forceActiveFocus()
    }

    function openCreateFolderDialog() {
        createFolderDialog.open()
    }

    function currentRowCount() {
        var flick = activeFlickable()
        return (flick && flick.count !== undefined) ? flick.count : 0
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
        var flick = activeFlickable()
        if (flick && typeof flick.positionViewAtIndex === "function") {
            flick.positionViewAtIndex(index, (controller.viewMode === "grid" ? GridView.Contain : ListView.Contain))
        }
    }

    function activeFlickable() {
        return viewLoader.item
    }

    function isViewInteracting() {
        const flick = activeFlickable()
        return !!(flick && (flick.moving || flick.flicking))
    }

    function allowHoverTooltips() {
        return !isViewInteracting()
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

    function toggleSort(field) {
        if (viewSettings.sortField === field) {
            viewSettings.sortAscending = !viewSettings.sortAscending
        } else {
            viewSettings.sortField = field
            viewSettings.sortAscending = true
        }
        if (controller && controller.fileModel) {
            controller.fileModel.setSortBy(viewSettings.sortField, viewSettings.sortAscending)
        }
    }

    function gridStepForUpDown() {
        var flick = activeFlickable()
        if (!flick || controller.viewMode !== "grid") return 1
        var cols = Math.floor(flick.width / flick.cellWidth)
        return Math.max(1, cols)
    }

    function openBulkRename() {
        if (!controller) return
        let selectedMetadata = []
        for (let i = 0; i < controller.selectedPaths.length; i++) {
            let meta = controller.fileModel.metadataForPath(controller.selectedPaths[i])
            if (Object.keys(meta).length > 0) {
                selectedMetadata.push(meta)
            }
        }
        if (selectedMetadata.length > 0) {
            bulkRenameDialog.controller = controller
            bulkRenameDialog.show(selectedMetadata)
        }
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

    function normalizePath(path) {
        if (!path) return ""
        var p = String(path)
        if (p.startsWith("file://")) {
            p = decodeURIComponent(p.substring("file://".length))
        }
        return p
    }

    function maxScrollY(view) {
        return Math.max(view.originY, view.originY + view.contentHeight - view.height)
    }

    function applyTouchpadScroll(view, event) {
        if (!view || !event) return
        var sens = (generalSettings && generalSettings.touchpadScrollSensitivity !== undefined)
            ? generalSettings.touchpadScrollSensitivity : 3.0
        var smoothing = (generalSettings && generalSettings.touchpadSmoothing !== undefined)
            ? Math.max(0.0, Math.min(0.9, generalSettings.touchpadSmoothing)) : 0.45
        var maxStepRatio = (generalSettings && generalSettings.touchpadMaxStepRatio !== undefined)
            ? Math.max(0.05, Math.min(0.35, generalSettings.touchpadMaxStepRatio)) : 0.14
        var zoomComp = Math.max(0.95, Math.min(1.25, root.zoomScale))
        var direction = event.inverted ? -1.0 : 1.0

        var dy = 0.0
        var dx = 0.0
        if (event.pixelDelta && (event.pixelDelta.y !== 0 || event.pixelDelta.x !== 0)) {
            dy = event.pixelDelta.y
            dx = event.pixelDelta.x
        } else if (event.angleDelta && event.angleDelta.y !== 0) {
            // Fallback for touchpads that only emit angle deltas.
            dy = (event.angleDelta.y / 120.0) * 42.0
            dx = event.angleDelta.x ? (event.angleDelta.x / 120.0) * 42.0 : 0.0
        } else {
            return
        }

        // Vertical intent only; damp diagonal/horizontal gestures to reduce accidental bursts.
        var axisMix = Math.abs(dy) / Math.max(1.0, Math.abs(dy) + Math.abs(dx))
        var stepRaw = dy * sens * zoomComp * direction * axisMix

        // Coalesce at frame cadence to avoid packet-level jitter.
        pendingTouchpadView = view
        pendingTouchpadDeltaY += stepRaw
        if (!touchpadScrollTimer.running) touchpadScrollTimer.start()

        // Store runtime params for timer tick.
        root._tpSmoothing = smoothing
        root._tpMaxStepRatio = maxStepRatio
        event.accepted = true
    }

    property real _tpSmoothing: 0.45
    property real _tpMaxStepRatio: 0.14

    Timer {
        id: touchpadScrollTimer
        interval: 16
        repeat: true
        onTriggered: {
            var view = root.pendingTouchpadView
            if (!view) {
                root.pendingTouchpadDeltaY = 0
                stop()
                return
            }
            var pending = root.pendingTouchpadDeltaY
            if (Math.abs(pending) < 0.05) {
                root.pendingTouchpadDeltaY = 0
                stop()
                return
            }

            var alpha = Math.max(0.12, 1.0 - root._tpSmoothing)
            var step = pending * alpha
            var maxStep = Math.max(22, view.height * root._tpMaxStepRatio)
            if (step > maxStep) step = maxStep
            if (step < -maxStep) step = -maxStep

            var nextY = Math.max(view.originY, Math.min(view.contentY - step, maxScrollY(view)))
            view.contentY = nextY
            root.pendingTouchpadDeltaY -= step
        }
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

        if (event.key === Qt.Key_Escape) {
            if (controller && controller.hasSelection) {
                controller.clearSelection()
                appWindow.toastManager.show("Selection cleared", false)
            }
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
            if (typeof globalThumbnailManager !== "undefined") setThumbnailManager(globalThumbnailManager)
            if (typeof globalPlacesModel !== "undefined") setPlacesModel(globalPlacesModel)
            var launchPath = initialPath && initialPath.length > 0 ? initialPath : homePath
            launchPath = normalizePath(launchPath)
            if (launchPath && launchPath.length > 0) openPath(launchPath)
            root.tabTitleChanged(controller.title)
            root.requestedActive()
            Qt.callLater(root.forceActiveFocus)
        }
        onRenameRequested: (path) => renamingPath = path
        onCurrentPathChanged: {
            root.tabTitleChanged(controller.title)
            Qt.callLater(root.forceActiveFocus)
        }
        onOperationSuccess: (msg) => {
            if (msg.indexOf("Compressing") !== -1 || msg.indexOf("Extracting") !== -1) {
                progressDialog.statusText = msg
                progressDialog.progress = 0.5
                progressDialog.open()
            } else {
                progressDialog.close()
                toastManager.show(msg, false)
            }
        }
        onOperationError: (msg) => {
            progressDialog.close()
            toastManager.show(msg, true)
        }
    }

    property var viewSettings: appWindow.viewSettings
    property var generalSettings: appWindow.generalSettings

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

    Menu {
        id: bgMenu
        Material.theme: (theme && theme.isDark) ? Material.Dark : Material.Light
        Material.background: theme.surfaceElevated
        MenuItem { text: "New Folder";   onTriggered: root.openCreateFolderDialog() }
        MenuItem { text: "Show Hidden";  checkable: true; checked: controller.showHiddenFiles; onTriggered: controller.showHiddenFiles = checked }
        MenuItem { text: "Paste";        enabled: controller.hasClipboard; onTriggered: controller.pasteItem() }
        MenuItem { text: "Go to Parent"; onTriggered: controller.goUp() }
        MenuItem { text: "Refresh";      onTriggered: controller.refresh() }
        MenuSeparator {}
        MenuItem {
            text: "Quit Liphis"
            onTriggered: Qt.quit()
            icon.source: "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/logout.svg"
        }
    }

    Component {
        id: listDelegate
        Item {
            id: listRoot
            width: ListView.view.width; height: baseListHeight * root.zoomScale
            readonly property bool isActuallySelected: model.isSelected !== undefined ? model.isSelected : false
            readonly property bool inClipboard: !!(controller && controller.clipboardPaths && controller.clipboardPaths.includes(model.path))
            readonly property bool clipboardCut: !!(controller && controller.isCutOp)
            readonly property bool isHidden: model.name !== undefined && model.name.startsWith(".")
            opacity: isHidden ? 0.6 : 1.0

            Rectangle {
                anchors.fill: parent; anchors.margins: 2; radius: theme ? theme.radiusSmall : 6
                color: listRoot.isActuallySelected ? theme.selection : (listMA.containsMouse ? theme.hover : "transparent")
                visible: listRoot.isActuallySelected || listMA.containsMouse
            }
            Rectangle {
                anchors.fill: parent; anchors.margins: 2; radius: theme ? theme.radiusSmall : 6
                visible: listRoot.inClipboard
                color: listRoot.clipboardCut ? Qt.rgba(0.85, 0.45, 0.15, 0.16) : Qt.rgba(0.15, 0.55, 0.85, 0.12)
                border.width: 1
                border.color: listRoot.clipboardCut ? Qt.rgba(0.85, 0.45, 0.15, 0.55) : Qt.rgba(0.15, 0.55, 0.85, 0.45)
            }

            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 0

                Item {
                    Layout.preferredWidth: (viewSettings ? viewSettings.colNameWidth : 300) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    RowLayout {
                        anchors.fill: parent; spacing: 12
                        FileIcon {
                            Layout.preferredWidth: 22 * (root.zoomScale || 1.0)
                            Layout.preferredHeight: 22 * (root.zoomScale || 1.0)
                            isDir: (model.isDir !== undefined ? model.isDir : false)
                            fileName: (model.name !== undefined ? String(model.name) : "")
                            iconName: (model.iconName !== undefined ? model.iconName : "")
                            theme: root.theme
                            gitStatus: (controller && model.path) ? (controller.gitStatus[model.path] || "") : ""
                        }
                        Text {
                            text: (model.name !== undefined ? root.displayName(model.name) : "")
                            textFormat: Text.PlainText
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: root.colTextPrimary
                            font.pixelSize: Math.max(8, 13 * (root.zoomScale || 1.0))
                            font.weight: listRoot.isActuallySelected ? Font.Medium : Font.Normal
                        }
                        ToolTip.visible: listMA.containsMouse && root.allowHoverTooltips()
                        ToolTip.delay: 450
                        ToolTip.timeout: 1200
                        ToolTip.text: (model.path !== undefined ? String(model.path) : "")
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showSize : true
                    Layout.preferredWidth: (viewSettings ? viewSettings.colSizeWidth : 100) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; rightPadding: 12
                        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
                        text: (model.formattedSize !== undefined ? model.formattedSize : "")
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showType : true
                    Layout.preferredWidth: (viewSettings ? viewSettings.colTypeWidth : 140) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; leftPadding: 12; verticalAlignment: Text.AlignVCenter
                        text: (model.type !== undefined ? model.type : "")
                        elide: Text.ElideRight
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showDate : true
                    Layout.preferredWidth: (viewSettings ? viewSettings.colDateWidth : 180) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; rightPadding: 12
                        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
                        text: (model.formattedDate !== undefined ? model.formattedDate : "")
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showCreated : false
                    Layout.preferredWidth: (viewSettings ? viewSettings.colCreatedWidth : 180) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; rightPadding: 12
                        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
                        text: (model.formattedCTime !== undefined ? model.formattedCTime : "")
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showAccessed : false
                    Layout.preferredWidth: (viewSettings ? viewSettings.colAccessedWidth : 180) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; rightPadding: 12
                        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
                        text: (model.formattedATime !== undefined ? model.formattedATime : "")
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showPermissions : false
                    Layout.preferredWidth: (viewSettings ? viewSettings.colPermsWidth : 100) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; leftPadding: 12; verticalAlignment: Text.AlignVCenter
                        text: (model.permissions !== undefined ? model.permissions : "")
                        font.family: "Monospace"
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 11 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showOwner : false
                    Layout.preferredWidth: (viewSettings ? viewSettings.colOwnerWidth : 100) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; leftPadding: 12; verticalAlignment: Text.AlignVCenter
                        text: (model.owner !== undefined ? model.owner : "")
                        elide: Text.ElideRight
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showGroup : false
                    Layout.preferredWidth: (viewSettings ? viewSettings.colGroupWidth : 100) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; leftPadding: 12; verticalAlignment: Text.AlignVCenter
                        text: (model.group !== undefined ? model.group : "")
                        elide: Text.ElideRight
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 12 * (root.zoomScale || 1.0))
                    }
                }

                Item {
                    visible: viewSettings ? viewSettings.showMime : false
                    Layout.preferredWidth: (viewSettings ? viewSettings.colMimeWidth : 180) * (root.zoomScale || 1.0)
                    Layout.fillHeight: true
                    Text {
                        anchors.fill: parent; leftPadding: 12; verticalAlignment: Text.AlignVCenter
                        text: (model.mimeType !== undefined ? model.mimeType : "")
                        elide: Text.ElideRight
                        color: root.colTextSecondary
                        font.pixelSize: Math.max(8, 11 * (root.zoomScale || 1.0))
                    }
                }

                Item { Layout.fillWidth: true }
            }

            MouseArea {
                id: listMA
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                preventStealing: false
                drag.target: dragProxy
                drag.threshold: 12

                Item {
                    id: dragProxy
                    Drag.active: listMA.drag.active
                    Drag.source: listRoot
                    Drag.supportedActions: Qt.CopyAction | Qt.MoveAction
                    Drag.keys: ["text/uri-list"]
                    Drag.mimeData: { "text/uri-list": controller.selectedPaths.join("\n") }
                }

                onClicked: (mouse) => {
                    if (root.isViewInteracting()) return
                    var idx = controller.indexOfPath(model.path)
                    root.keyboardIndex = idx
                    root.forceActiveFocus()
                    if (mouse.button === Qt.RightButton) {
                        if (typeof itemMenuHandler === "function") itemMenuHandler(model, controller)
                    } else if (mouse.button === Qt.LeftButton && generalSettings.singleClick) {
                        if (mouse.modifiers & Qt.ShiftModifier) {
                            if (root.rangeAnchorIndex < 0) root.rangeAnchorIndex = idx
                            controller.selectRangeByIndexes(root.rangeAnchorIndex, idx)
                        } else if (mouse.modifiers & Qt.ControlModifier) {
                            controller.toggleSelection(model.path)
                        } else {
                            controller.selectPath(model.path)
                        }
                        if (!(mouse.modifiers & Qt.ShiftModifier)) root.rangeAnchorIndex = root.keyboardIndex
                        controller.openPath(model.path)
                    } else if (mouse.button === Qt.LeftButton) {
                        if (mouse.modifiers & Qt.ShiftModifier) {
                            if (root.rangeAnchorIndex < 0) root.rangeAnchorIndex = idx
                            controller.selectRangeByIndexes(root.rangeAnchorIndex, idx)
                        } else if (mouse.modifiers & Qt.ControlModifier) {
                            controller.toggleSelection(model.path)
                        } else {
                            controller.selectPath(model.path)
                        }
                        if (!(mouse.modifiers & Qt.ShiftModifier)) root.rangeAnchorIndex = root.keyboardIndex
                    }
                }
                onDoubleClicked: if (!generalSettings.singleClick) controller.openPath(model.path)
            }

            DropArea {
                anchors.fill: parent
                enabled: model.isDir
                onDropped: (drop) => {
                    if (drop.hasText) {
                        let paths = drop.text.split("\n")
                        controller.dropItems(paths, model.path, (drop.supportedActions & Qt.CopyAction) && (drop.proposedAction === Qt.CopyAction))
                    }
                }
            }
        }
    }

    Component {
        id: gridDelegate
        Item {
            id: gridRoot
            // We use the cellWidth/Height from the GridView that is loading us.
            // Since GridView sets width/height of delegates to cellWidth/Height by default,
            // we can just fill parent or rely on the parent sizing.
            implicitWidth: 100; implicitHeight: 100
            readonly property bool isActuallySelected: model.isSelected !== undefined ? model.isSelected : false
            readonly property bool inClipboard: !!(controller && controller.clipboardPaths && controller.clipboardPaths.includes(model.path))
            readonly property bool clipboardCut: !!(controller && controller.isCutOp)
            readonly property bool isHidden: model.name !== undefined && model.name.startsWith(".")
            opacity: isHidden ? 0.6 : 1.0

            Rectangle {
                anchors.fill: parent; anchors.margins: 4; radius: theme ? theme.radius : 8
                color: gridRoot.isActuallySelected ? theme.selection : (gridMA.containsMouse ? theme.hover : "transparent")
                visible: gridRoot.isActuallySelected || gridMA.containsMouse
            }
            Rectangle {
                anchors.fill: parent; anchors.margins: 4; radius: theme ? theme.radius : 8
                visible: gridRoot.inClipboard
                color: gridRoot.clipboardCut ? Qt.rgba(0.85, 0.45, 0.15, 0.16) : Qt.rgba(0.15, 0.55, 0.85, 0.12)
                border.width: 1
                border.color: gridRoot.clipboardCut ? Qt.rgba(0.85, 0.45, 0.15, 0.55) : Qt.rgba(0.15, 0.55, 0.85, 0.45)
            }

            ColumnLayout {
                anchors.fill: parent; anchors.margins: Math.max(3, 5 * (zoomLevel / 100.0)); spacing: 0
                Item {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    FileIcon {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        width: Math.min(parent.width, baseIconSize * (zoomLevel / 100.0))
                        height: Math.min(parent.height, baseIconSize * (zoomLevel / 100.0))
                        isDir: (model.isDir !== undefined ? model.isDir : false)
                        fileName: (model.name !== undefined ? String(model.name) : "")
                        iconName: (model.iconName !== undefined ? model.iconName : "")
                        theme: root.theme
                        visible: !gridThumb.visible
                        gitStatus: (controller && model.path) ? (controller.gitStatus[model.path] || "") : ""
                    }
                    Image {
                        id: gridThumb
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                        source: (model.thumbnail !== undefined ? model.thumbnail : "")
                        sourceSize.width: Math.max(128, width)
                        sourceSize.height: Math.max(128, height)
                        visible: status === Image.Ready
                        asynchronous: true
                        cache: true
                    }
                }
                Text {
                    text: (model.name !== undefined ? root.displayName(model.name) : "")
                    textFormat: Text.PlainText
                    Layout.fillWidth: true
                    Layout.preferredHeight: font.pixelSize * 2.1
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                    wrapMode: gridRoot.isActuallySelected ? Text.WrapAnywhere : Text.NoWrap
                    maximumLineCount: 2
                    color: root.colTextPrimary
                    font.pixelSize: Math.max(8, 8 + (4.5 * (zoomLevel / 100.0)))
                    font.weight: gridRoot.isActuallySelected ? Font.Medium : Font.Normal
                }
                ToolTip.visible: gridMA.containsMouse && root.allowHoverTooltips()
                ToolTip.delay: 450
                ToolTip.timeout: 1200
                ToolTip.text: (model.path !== undefined ? String(model.path) : "")
            }

            MouseArea {
                id: gridMA
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                preventStealing: false
                drag.target: gridDragProxy
                drag.threshold: 12

                Item {
                    id: gridDragProxy
                    Drag.active: gridMA.drag.active
                    Drag.source: gridRoot
                    Drag.supportedActions: Qt.CopyAction | Qt.MoveAction
                    Drag.keys: ["text/uri-list"]
                    Drag.mimeData: { "text/uri-list": controller.selectedPaths.join("\n") }
                }

                onClicked: (mouse) => {
                    if (root.isViewInteracting()) return
                    var idx = controller.indexOfPath(model.path)
                    root.keyboardIndex = idx
                    root.forceActiveFocus()
                    if (mouse.button === Qt.RightButton) {
                        if (typeof itemMenuHandler === "function") itemMenuHandler(model, controller)
                    } else if (mouse.button === Qt.LeftButton && generalSettings.singleClick) {
                        if (mouse.modifiers & Qt.ShiftModifier) {
                            if (root.rangeAnchorIndex < 0) root.rangeAnchorIndex = idx
                            controller.selectRangeByIndexes(root.rangeAnchorIndex, idx)
                        } else if (mouse.modifiers & Qt.ControlModifier) {
                            controller.toggleSelection(model.path)
                        } else {
                            controller.selectPath(model.path)
                        }
                        if (!(mouse.modifiers & Qt.ShiftModifier)) root.rangeAnchorIndex = root.keyboardIndex
                        controller.openPath(model.path)
                    } else if (mouse.button === Qt.LeftButton) {
                        if (mouse.modifiers & Qt.ShiftModifier) {
                            if (root.rangeAnchorIndex < 0) root.rangeAnchorIndex = idx
                            controller.selectRangeByIndexes(root.rangeAnchorIndex, idx)
                        } else if (mouse.modifiers & Qt.ControlModifier) {
                            controller.toggleSelection(model.path)
                        } else {
                            controller.selectPath(model.path)
                        }
                        if (!(mouse.modifiers & Qt.ShiftModifier)) root.rangeAnchorIndex = root.keyboardIndex
                    }
                }
                onDoubleClicked: if (!generalSettings.singleClick) controller.openPath(model.path)
            }

            DropArea {
                anchors.fill: parent
                enabled: model.isDir
                onDropped: (drop) => {
                    if (drop.hasText) {
                        let paths = drop.text.split("\n")
                        controller.dropItems(paths, model.path, (drop.supportedActions & Qt.CopyAction) && (drop.proposedAction === Qt.CopyAction))
                    }
                }
            }
        }
    }

    Loader {
        id: viewLoader
        anchors.fill: parent
        focus: true
        sourceComponent: {
            if (controller.viewMode === "grid") return gridComponent;
            if (controller.viewMode === "tree") return treeComponent;
            return listComponent;
        }
    }

    Component {
        id: listComponent
        ListView {
            id: lv
            anchors.fill: parent
            model: controller.fileModel
            clip: true
            delegate: listDelegate
            focus: true
            interactive: true
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 1500
            maximumFlickVelocity: 15000
            pixelAligned: false
            WheelHandler {
                acceptedDevices: PointerDevice.TouchPad
                blocking: true
                onWheel: (event) => applyTouchpadScroll(lv, event)
            }

            Keys.onEscapePressed: (event) => {
                if (controller) { controller.clearSelection(); event.accepted = true }
            }

            reuseItems: true
            cacheBuffer: 400
            ScrollBar.vertical:   ScrollBar { policy: ScrollBar.AsNeeded; active: true }
            ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }

            headerPositioning: ListView.OverlayHeader
            header: Rectangle {
                z: 10; width: lv.width; height: 36; color: theme.surfaceMuted

                Row {
                    anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 0

                    HeaderColumn {
                        title: "Name"; field: "name"; theme: root.theme; minWidth: 100
                        columnWidth: viewSettings.colNameWidth * root.zoomScale
                        paddingLeft: 34 * root.zoomScale
                        isSortActive: viewSettings.sortField === "name"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("name")
                        onWidthChangedByHandle: (w) => viewSettings.colNameWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showSize; title: "Size"; field: "size"; theme: root.theme; minWidth: 60
                        columnWidth: viewSettings.colSizeWidth * root.zoomScale
                        horizontalAlignment: Text.AlignRight; paddingRight: 12
                        isSortActive: viewSettings.sortField === "size"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("size")
                        onWidthChangedByHandle: (w) => viewSettings.colSizeWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showType; title: "Type"; field: "type"; theme: root.theme; minWidth: 60
                        columnWidth: viewSettings.colTypeWidth * root.zoomScale
                        paddingLeft: 12
                        isSortActive: viewSettings.sortField === "type"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("type")
                        onWidthChangedByHandle: (w) => viewSettings.colTypeWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showDate; title: "Date Modified"; field: "date"; theme: root.theme; minWidth: 80
                        columnWidth: viewSettings.colDateWidth * root.zoomScale
                        horizontalAlignment: Text.AlignRight; paddingRight: 12
                        isSortActive: viewSettings.sortField === "date"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("date")
                        onWidthChangedByHandle: (w) => viewSettings.colDateWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showCreated; title: "Date Created"; field: "ctime"; theme: root.theme; minWidth: 80
                        columnWidth: viewSettings.colCreatedWidth * root.zoomScale
                        horizontalAlignment: Text.AlignRight; paddingRight: 12
                        isSortActive: viewSettings.sortField === "ctime"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("ctime")
                        onWidthChangedByHandle: (w) => viewSettings.colCreatedWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showAccessed; title: "Date Accessed"; field: "atime"; theme: root.theme; minWidth: 80
                        columnWidth: viewSettings.colAccessedWidth * root.zoomScale
                        horizontalAlignment: Text.AlignRight; paddingRight: 12
                        isSortActive: viewSettings.sortField === "atime"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("atime")
                        onWidthChangedByHandle: (w) => viewSettings.colAccessedWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showPermissions; title: "Permissions"; field: "permissions"; theme: root.theme; minWidth: 60
                        columnWidth: viewSettings.colPermsWidth * root.zoomScale
                        paddingLeft: 12
                        isSortActive: viewSettings.sortField === "permissions"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("permissions")
                        onWidthChangedByHandle: (w) => viewSettings.colPermsWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showOwner; title: "Owner"; field: "owner"; theme: root.theme; minWidth: 60
                        columnWidth: viewSettings.colOwnerWidth * root.zoomScale
                        paddingLeft: 12
                        isSortActive: viewSettings.sortField === "owner"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("owner")
                        onWidthChangedByHandle: (w) => viewSettings.colOwnerWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showGroup; title: "Group"; field: "group"; theme: root.theme; minWidth: 60
                        columnWidth: viewSettings.colGroupWidth * root.zoomScale
                        paddingLeft: 12
                        isSortActive: viewSettings.sortField === "group"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("group")
                        onWidthChangedByHandle: (w) => viewSettings.colGroupWidth = w / root.zoomScale
                    }
                    HeaderColumn {
                        visible: viewSettings.showMime; title: "MIME Type"; field: "mimeType"; theme: root.theme; minWidth: 80
                        columnWidth: viewSettings.colMimeWidth * root.zoomScale
                        paddingLeft: 12; showSplitter: false
                        isSortActive: viewSettings.sortField === "mimeType"
                        sortAscending: viewSettings.sortAscending
                        onClicked: toggleSort("mimeType")
                        onWidthChangedByHandle: (w) => viewSettings.colMimeWidth = w / root.zoomScale
                    }
                    Item { width: 40; height: 1 } // Spacer
                }

                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }
            }
        }
    }

    Component {
        id: gridComponent
        GridView {
            id: gv
            anchors.fill: parent
            model: controller.fileModel
            clip: true
            delegate: gridDelegate
            focus: true
            interactive: true
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 1500
            maximumFlickVelocity: 15000
            pixelAligned: false
            WheelHandler {
                acceptedDevices: PointerDevice.TouchPad
                blocking: true
                onWheel: (event) => applyTouchpadScroll(gv, event)
            }

            Keys.onEscapePressed: (event) => {
                if (controller) { controller.clearSelection(); event.accepted = true }
            }

            cellWidth:  Math.max(102, baseCellSize * root.zoomScale)
            cellHeight: Math.max(108, baseCellSize * root.zoomScale)
            reuseItems: true
            cacheBuffer: 800
            ScrollBar.vertical:   ScrollBar { policy: ScrollBar.AsNeeded }
            ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }
        }
    }

    Component {
        id: treeComponent
        TreeView {
            id: treeView
            anchors.fill: parent
            model: controller.treeModel
            focus: true
            clip: true
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 1500
            maximumFlickVelocity: 15000
            pixelAligned: false
            WheelHandler {
                acceptedDevices: PointerDevice.TouchPad
                blocking: true
                onWheel: (event) => applyTouchpadScroll(treeView, event)
            }

            delegate: Item {
                id: treeDelegate
                implicitWidth: treeView.implicitWidth
                implicitHeight: 32 * root.zoomScale

                readonly property bool isActuallySelected: model.isSelected !== undefined ? model.isSelected : false
                readonly property bool inClipboard: !!(controller && controller.clipboardPaths && controller.clipboardPaths.includes(model.path))
                readonly property bool clipboardCut: !!(controller && controller.isCutOp)

                Rectangle {
                    anchors.fill: parent; anchors.margins: 2; radius: 4
                    color: isActuallySelected ? theme.selection : (treeMA.containsMouse ? theme.hover : "transparent")
                    visible: isActuallySelected || treeMA.containsMouse
                }
                Rectangle {
                    anchors.fill: parent; anchors.margins: 2; radius: 4
                    visible: inClipboard
                    color: clipboardCut ? Qt.rgba(0.85, 0.45, 0.15, 0.16) : Qt.rgba(0.15, 0.55, 0.85, 0.12)
                    border.width: 1
                    border.color: clipboardCut ? Qt.rgba(0.85, 0.45, 0.15, 0.55) : Qt.rgba(0.15, 0.55, 0.85, 0.45)
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: (model.depth || 0) * (20 * root.zoomScale) + (8 * root.zoomScale)
                    spacing: 8 * root.zoomScale

                    Icon {
                        name: model.isExpanded ? "chevron-down" : "chevron-right"
                        iconSize: 12 * root.zoomScale
                        color: theme.textTertiary
                        visible: model.hasChildren
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (model.isExpanded) treeView.collapse(model.index)
                                else treeView.expand(model.index)
                            }
                        }
                    }

                    FileIcon {
                        Layout.preferredWidth: 18 * root.zoomScale
                        Layout.preferredHeight: 18 * root.zoomScale
                        isDir: model.isDir
                        fileName: model.name
                        iconName: model.iconName
                        theme: root.theme
                        gitStatus: (controller && model.path) ? (controller.gitStatus[model.path] || "") : ""
                    }

                    Text {
                        text: model.name
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        color: treeDelegate.isActuallySelected ? theme.accent : theme.textPrimary
                        font.pixelSize: 13 * root.zoomScale
                    }
                    ToolTip.visible: treeMA.containsMouse && root.allowHoverTooltips()
                    ToolTip.delay: 450
                    ToolTip.timeout: 1200
                    ToolTip.text: model.path
                }

                MouseArea {
                    id: treeMA
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: false
                    onClicked: {
                        if (root.isViewInteracting()) return
                        controller.selectPath(model.path)
                        if (model.isDir && generalSettings.singleClick) controller.openPath(model.path)
                    }
                    onDoubleClicked: if (model.isDir && !generalSettings.singleClick) controller.openPath(model.path)
                }
            }

            ScrollBar.vertical:   ScrollBar { policy: ScrollBar.AsNeeded }
            ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bg
        visible: controller.loading || controller.searchInProgress
        opacity: 0.6
        z: 5

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 12
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: parent.parent.visible
            }
            Text {
                text: controller.searchInProgress ? "Searching..." : "Loading..."
                color: theme.textSecondary
                font.pixelSize: 13
            }
        }
    }

    Item {
        id: emptyState
        anchors.fill: parent
        visible: !controller.loading && !controller.searchInProgress && controller.fileModel.count === 0
        z: 4

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20

            Icon {
                Layout.alignment: Qt.AlignHCenter
                name: "search"
                iconSize: 64
                color: theme.textMuted
                opacity: 0.4
            }

            Column {
                Layout.alignment: Qt.AlignHCenter
                spacing: 4
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: controller.activeSearchTerm ? "No results found" : "Folder is empty"
                    color: theme.textPrimary
                    font.pixelSize: 18
                    font.weight: Font.Medium
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: controller.activeSearchTerm
                        ? "Try different keywords or filters"
                        : "This directory contains no files"
                    color: theme.textSecondary
                    font.pixelSize: 13
                }
            }
        }
    }

    Connections {
        target: controller
        function onCurrentPathChanged() {
            Qt.callLater(root.forceActiveFocus)
        }
        function onLoadingChanged() {
            if (controller && !controller.loading) Qt.callLater(root.forceActiveFocus)
        }
    }

    Connections {
        target: controller ? controller.fileModel : null
    }

    MouseArea {
        anchors.fill: parent
        z: -1
        acceptedButtons: Qt.RightButton | Qt.BackButton | Qt.ForwardButton | Qt.LeftButton
        onClicked: (mouse) => {
            root.forceActiveFocus()
            if      (mouse.button === Qt.RightButton)   bgMenu.popup()
            else if (mouse.button === Qt.BackButton)    controller.goBack()
            else if (mouse.button === Qt.ForwardButton) controller.goForward()
            else if (mouse.button === Qt.LeftButton)    controller.clearSelection()
        }
    }

    Item {
        anchors.fill: parent
        z: 1000

        PinchHandler {
            target: null
            property real startZoom: 100
            enabled: false
            onActiveChanged: if (active) startZoom = root.zoomLevel
            onScaleChanged: {
                var newZoom = Math.max(50, Math.min(300, startZoom * scale))
                root.queueZoom(newZoom)
            }
        }
    }
}
