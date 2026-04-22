import QtQuick

Item {
    id: root
    property bool isDir: false
    property string iconName: ""
    property var theme: null
    property string gitStatus: ""

    readonly property string resolvedIconName: {
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

    Image {
        id: folderImage
        anchors.fill: parent
        source: root.customFolderIcon
        visible: root.isCustomFolder
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignBottom
        smooth: true
        asynchronous: true
    }

    Icon {
        id: themedIcon
        anchors.fill: parent
        name: root.resolvedIconName
        filled: root.isDir
        visible: !root.isCustomFolder
        color: isDir
            ? (theme ? theme.accent : "#4A473E")
            : (theme ? theme.textSecondary : "#8A867E")
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
