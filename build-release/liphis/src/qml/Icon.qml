import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    id: root
    property string name: ""
    property color color: "white"
    property int iconSize: 24
    property bool useVector: true
    property bool filled: false

    implicitWidth: iconSize
    implicitHeight: iconSize
    width: iconSize
    height: iconSize

    function mappedIconName(iconName) {
        if (iconName === "back") return "go-previous"
        if (iconName === "forward") return "go-next"
        if (iconName === "up") return "go-up"
        if (iconName === "refresh") return "view-refresh"
        if (iconName === "search") return "edit-find"
        if (iconName === "grid") return "view-grid"
        if (iconName === "list") return "view-list-details"
        if (iconName === "dual") return "view-split-left-right"
        if (iconName === "terminal") return "utilities-terminal"
        if (iconName === "remote") return "network-server"
        if (iconName === "home" || iconName === "user-home") return "go-home"
        if (iconName === "close") return "window-close"
        if (iconName === "trash") return "user-trash"
        if (iconName === "folder" || iconName.indexOf("folder-") === 0) return iconName
        if (iconName === "doc") return "text-x-generic"
        if (iconName === "pic" || iconName === "folder-pictures") return "folder-pictures"
        if (iconName === "star" || iconName === "user-bookmarks") return "bookmark-new"
        if (iconName === "drive-harddisk-system") return "drive-harddisk-system"
        if (iconName === "drive-removable-media") return "drive-removable-media"
        if (iconName === "document-open-recent") return "document-open-recent"
        if (iconName === "down") return "go-down"
        if (iconName === "settings") return "settings"
        if (iconName === "info") return "dialog-information"
        if (iconName === "tag") return "tag"
        return iconName && iconName.length > 0 ? iconName : "text-x-generic"
    }

    function localAsset(iconName) {
        var n = iconName || ""
        if (n === "back") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/arrow-left.svg"
        if (n === "forward") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/arrow-right.svg"
        if (n === "up") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/arrow-up.svg"
        if (n === "refresh" || n === "view-refresh") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/refresh.svg"
        if (n === "search" || n === "edit-find") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/search.svg"
        if (n === "home" || n === "user-home" || n === "go-home") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/home.svg"
        if (n === "star" || n === "bookmark-new") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/star.svg"
        if (n === "star-filled") return "qrc:/qt/qml/liphis/src/qml/assets/icons/filled/star.svg"
        if (n === "user-bookmarks") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/bookmark.svg"
        if (n === "close" || n === "window-close") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/x.svg"
        if (n === "folder" || n === "folder-open" || n.indexOf("folder-") === 0) return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder.svg"
        if (n === "info" || n === "dialog-information") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/info-circle.svg"
        if (n === "terminal" || n === "utilities-terminal") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/terminal.svg"
        if (n === "edit-copy") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/copy.svg"
        if (n === "edit-cut") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/scissors.svg"
        if (n === "edit-rename") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/pencil.svg"
        if (n === "user-trash" || n === "trash") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/trash.svg"
        if (n === "view-list-details") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/list-details.svg"
        if (n === "tab-new") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/square-plus.svg"
        if (n === "window-new") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/app-window.svg"
        if (n === "view-split-left-right") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-board-split.svg"
        if (n === "dialog-password") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/password.svg"
        if (n === "vscode") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/brand-vscode.svg"
        if (n === "media-playback-start") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/player-play.svg"
        if (n === "background") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/background.svg"
        if (n === "tag") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/tag.svg"
        if (n === "document-open") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-open.svg"
        if (n === "document-properties") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/list-details.svg"
        if (n === "layout-grid") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-grid.svg"
        if (n === "layout-list") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-list.svg"
        if (n === "layout-tree") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-tree.svg"
        if (n === "layout-sidebar") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-sidebar.svg"
        if (n === "layout-sidebar-left-collapse") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-sidebar-left-collapse.svg"
        if (n === "layout-sidebar-left-expand") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/layout-sidebar-left-expand.svg"
        if (n === "alphabet-latin") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/alphabet-latin.svg"
        if (n === "calendar") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/calendar.svg"
        if (n === "database") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/database.svg"
        if (n === "eye") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/eye.svg"
        if (n === "zoom-in") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/zoom-in.svg"
        if (n === "zoom-out") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/zoom-out.svg"
        if (n === "zoom-reset") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/zoom-reset.svg"
        if (n === "clipboard") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/clipboard.svg"
        if (n === "logout") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/logout.svg"
        if (n === "chart-pie") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/chart-pie.svg"
        if (n === "folder-code") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-code.svg"
        if (n === "folder-heart") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-heart.svg"
        if (n === "folder-star") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-star.svg"
        if (n === "folder-plus") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-plus.svg"
        if (n === "folder-root") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/folder-root.svg"
        if (n === "device-desktop") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/device-desktop.svg"
        if (n === "device-floppy") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/device-floppy.svg"
        if (n === "device-sd-card") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/device-sd-card.svg"
        if (n === "device-usb") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/device-usb.svg"
        if (n === "music") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/music.svg"
        if (n === "photo") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/photo.svg"
        if (n === "video") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/video.svg"
        if (n === "download") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/download.svg"
        if (n === "chevron-right") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/chevron-right.svg"
        if (n === "chevron-down") return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/chevron-down.svg"
        return ""
    }

    function filledAsset(iconName) {
        var n = iconName || ""
        if (n === "folder" || n === "folder-open" || n.indexOf("folder-") === 0) return "qrc:/qt/qml/liphis/src/qml/assets/icons/filled/folder.svg"
        if (n === "star") return "qrc:/qt/qml/liphis/src/qml/assets/icons/filled/star.svg"
        return ""
    }

    readonly property string iconTheme: (typeof generalSettings !== "undefined" && generalSettings) ? generalSettings.iconTheme : "outline"
    readonly property string assetSource: (iconTheme === "filled" || root.filled) ? (filledAsset(root.name) || localAsset(root.name)) : localAsset(root.name)

    Image {
        id: sourceIcon
        anchors.fill: parent
        source: root.assetSource.length > 0
            ? root.assetSource
            : "image://icon/" + root.mappedIconName(root.name)
        sourceSize.width: Math.max(32, Math.round(root.width * 1.5))
        sourceSize.height: Math.max(32, Math.round(root.height * 1.5))
        fillMode: Image.PreserveAspectFit
        smooth: root.width > 32
        mipmap: true
        antialiasing: true
    }

    ColorOverlay {
        anchors.fill: sourceIcon
        source: sourceIcon
        visible: !root.useVector
        color: root.color
    }
}
