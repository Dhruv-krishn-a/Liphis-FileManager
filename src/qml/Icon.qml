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
        if (n === "back") n = "arrow-left"
        if (n === "forward") n = "arrow-right"
        if (n === "up") n = "arrow-up"
        if (n === "down") n = "arrow-down"
        if (n === "close" || n === "window-close") n = "x"
        if (n === "home" || n === "user-home" || n === "go-home") n = "home"
        if (n === "edit-rename") n = "pencil"
        if (n === "user-trash" || n === "trash") n = "trash"
        if (n === "view-list-details") n = "list-details"
        if (n === "tab-new") n = "square-plus"
        if (n === "window-new") n = "app-window"
        if (n === "view-split-left-right") n = "layout-board-split"
        if (n === "media-playback-start") n = "player-play"
        if (n === "mouse-pointer" || n === "pointer") n = "pointer"
        if (n === "preferences-system") n = "settings"
        if (n === "info" || n === "dialog-information") n = "info-circle"
        if (n === "doc" || n === "text-x-generic" || n === "text-plain") n = "file-text"
        if (n === "layout-tree") n = "list-tree"
        
        // Final absolute path in resource tree based on CMake GLOB
        return "qrc:/qt/qml/liphis/src/qml/assets/icons/outline/" + n + ".svg"
    }

    function filledAsset(iconName) {
        var n = iconName || ""
        if (n === "folder-open" || n === "folder" || n.indexOf("folder-") === 0) n = "folder"
        if (n === "star") n = "star"
        return "qrc:/qt/qml/liphis/src/qml/assets/icons/filled/" + n + ".svg"
    }

    function prefersThemeProvider(iconName) {
        var n = iconName || ""
        if (!n.length) return false
        if (n.indexOf("/") !== -1) return true // MIME type like application/pdf

        var mimeLikePrefixes = [
            "application-",
            "audio-",
            "font-",
            "image-",
            "inode-",
            "message-",
            "model-",
            "multipart-",
            "package-",
            "text-",
            "video-",
            "x-content-"
        ]
        for (var i = 0; i < mimeLikePrefixes.length; ++i) {
            if (n.indexOf(mimeLikePrefixes[i]) === 0)
                return true
        }

        var themeNames = {
            "document-open-recent": true,
            "drive-harddisk-system": true,
            "drive-removable-media": true
        }
        return !!themeNames[n]
    }

    readonly property string iconTheme: (typeof generalSettings !== "undefined" && generalSettings) ? generalSettings.iconTheme : "outline"
    readonly property string assetSource: "image://icon/" + root.mappedIconName(root.name)

    Image {
        id: sourceIcon
        anchors.fill: parent
        source: root.assetSource
        sourceSize.width: Math.max(16, root.width)
        sourceSize.height: Math.max(16, root.height)
        fillMode: Image.PreserveAspectFit
        asynchronous: true
        cache: true
        
        onStatusChanged: {
            if (status === Image.Error) {
                let local = root.localAsset(root.name)
                if (source.toString() !== local) {
                    source = local
                }
            }
        }
    }
}
