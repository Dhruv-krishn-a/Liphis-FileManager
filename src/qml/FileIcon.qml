import QtQuick

Item {
    id: root
    property bool isDir: false
    property string iconName: ""
    property string fileName: ""
    property var theme: null
    property string gitStatus: ""

    function fileTypeIconFromName(name) {
        var n = String(name || "")
        var dot = n.lastIndexOf(".")
        if (dot < 0) return ""
        var ext = n.substring(dot + 1).toLowerCase()

        if (["py", "pyw"].indexOf(ext) !== -1) return "brand-python"
        if (["cpp", "cc", "cxx", "c", "h", "hpp", "hh", "rs", "go", "java", "kt", "swift", "rb", "php", "sh", "bash", "zsh", "ts", "tsx", "js", "jsx", "vue", "html", "css", "scss", "sql", "xml", "yaml", "yml", "toml", "ini", "conf"].indexOf(ext) !== -1) return "file-code"
        if (ext === "json") return "json"
        if (ext === "ipynb") return "notebook"

        if (["doc", "docx", "odt", "rtf", "txt", "md"].indexOf(ext) !== -1) return "file-word"
        if (["xls", "xlsx", "ods", "csv"].indexOf(ext) !== -1) return "file-spreadsheet"
        if (["ppt", "pptx", "odp"].indexOf(ext) !== -1) return "file-description"
        if (ext === "pdf") return "file-type-pdf"

        if (["mp3", "wav", "flac", "ogg", "m4a", "aac"].indexOf(ext) !== -1) return "file-music"
        if (["mp4", "mkv", "mov", "avi", "webm", "m4v"].indexOf(ext) !== -1) return "video"
        if (["png", "jpg", "jpeg", "gif", "webp", "bmp", "svg"].indexOf(ext) !== -1) return "photo"
        if (["zip", "rar", "7z", "tar", "gz", "bz2", "xz"].indexOf(ext) !== -1) return "file-zip"
        return ""
    }

    readonly property string resolvedIconName: {
        var typed = fileTypeIconFromName(fileName)
        if (typed.length > 0) return typed
        if (iconName && iconName.length > 0) return iconName
        return isDir ? "folder" : "text-x-generic"
    }

    readonly property bool isCustomFolder: isDir && (iconName === "" || iconName === "folder")
    readonly property string customFolderIcon: {
        if (!isCustomFolder) return ""
        return theme && theme.isDark 
            ? "qrc:/qt/qml/liphis/src/qml/assets/folder-dark.png" 
            : "qrc:/qt/qml/liphis/src/qml/assets/folder-light.png"
    }
    readonly property real visualScale: isCustomFolder ? 0.94 : (isDir ? 0.86 : 0.72)
    readonly property int visualSize: Math.round(Math.min(width, height) * visualScale)

    Image {
        id: folderImage
        anchors.centerIn: parent
        width: root.visualSize
        height: root.visualSize
        source: root.customFolderIcon
        visible: root.isCustomFolder
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignBottom
        smooth: true
        asynchronous: true
    }

    Icon {
        id: themedIcon
        anchors.centerIn: parent
        width: root.visualSize
        height: root.visualSize
        name: root.resolvedIconName
        filled: false
        tint: true
        visible: !root.isCustomFolder
        color: root.isDir
            ? (theme ? theme.accent : "#B9783E")
            : (theme ? theme.textSecondary : "#6F685F")
    }

    // Git Status Badge
    Rectangle {
        id: badge
        visible: root.gitStatus !== "" && root.gitStatus !== "  "
        width: Math.max(6, parent.width * 0.35)
        height: width
        radius: width / 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: -width * 0.1
        anchors.bottomMargin: -width * 0.1
        border.width: 1.5
        border.color: theme ? theme.bg : "white"
        
        color: {
            if (!root.gitStatus) return "transparent"
            let s = root.gitStatus
            if (s.includes("??") || s.includes("A")) return theme ? theme.success : "green"
            if (s.includes("M")) return theme ? theme.warning : "orange"
            if (s.includes("D") || s.includes("U")) return theme ? theme.error : "red"
            return theme ? theme.textTertiary : "gray"
        }

        // Tooltip or small indicator for the exact letter could go here
    }
}
