import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    id: root
    property string name: ""
    property color color: "white"
    property int iconSize: 24
    property bool tint: true
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
        if (iconName === "settings" || iconName === "preferences") return "settings"
        if (iconName === "info") return "dialog-information"
        if (iconName === "tag") return "tag"
        return iconName && iconName.length > 0 ? iconName : "text-x-generic"
    }

    function localAsset(iconName, setName) {
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
        if (n === "text-x-log") n = "file-text"
        if (n === "layout-tree") n = "list-tree"
        if (n === "star-filled") n = "star"
        if (n === "document-open-recent") n = "history"
        if (n === "drive-harddisk" || n === "drive-harddisk-system" || n === "drive-removable-media") n = "database"
        if (n.indexOf("text-") === 0) n = "file-text"
        if (n.indexOf("image-") === 0) n = "photo"
        if (n.indexOf("audio-") === 0) n = "music"
        if (n.indexOf("video-") === 0) n = "video"
        if (n.indexOf("application-") === 0 || n.indexOf("model-") === 0 || n.indexOf("inode-") === 0 || n.indexOf("multipart-") === 0 || n.indexOf("package-") === 0) n = "file"
        
        return "qrc:/qt/qml/liphis/src/qml/assets/icons/" + (setName || "outline") + "/" + n + ".svg"
    }

    function prefersThemeProvider(iconName) {
        var n = iconName || ""
        if (!n.length) return false
        if (n.indexOf("/") !== -1) return true // MIME type like application/pdf

        var themeNames = {
            "network-server": true
        }
        return !!themeNames[n]
    }

    readonly property string iconTheme: (typeof generalSettings !== "undefined" && generalSettings) ? generalSettings.iconTheme : "outline"
    readonly property string mappedName: root.mappedIconName(root.name)
    readonly property bool themeProviderPreferred: root.prefersThemeProvider(mappedName)
    readonly property string preferredSet: iconTheme === "filled" ? "filled" : "outline"
    readonly property string preferredAssetSource: root.localAsset(root.name, preferredSet)
    readonly property string outlineAssetSource: root.localAsset(root.name, "outline")
    readonly property string themeSource: "image://icon/" + mappedName

    function sourceForMode() {
        // For UI controls, prefer bundled assets (deterministic and theme-aware).
        // For MIME/system names, prefer system theme provider.
        return themeProviderPreferred ? themeSource : preferredAssetSource
    }

    Image {
        id: sourceIcon
        anchors.fill: parent
        source: root.sourceForMode()
        sourceSize.width: Math.max(16, root.width)
        sourceSize.height: Math.max(16, root.height)
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        asynchronous: true
        cache: true
        
        onStatusChanged: {
            if (status === Image.Error) {
                var finalFallback = root.outlineAssetSource
                if (source.toString() !== finalFallback) {
                    source = finalFallback
                } else {
                    var guaranteed = root.localAsset("folder", "outline")
                    if (source.toString() !== guaranteed) {
                        source = guaranteed
                    }
                }
            }
        }
    }

    ColorOverlay {
        anchors.fill: sourceIcon
        source: sourceIcon
        color: root.color
        visible: root.tint
    }
}
