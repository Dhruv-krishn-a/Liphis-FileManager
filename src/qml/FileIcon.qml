import QtQuick

Item {
    id: root
    property bool isDir: false
    property string iconName: ""
    property var theme: null

    readonly property string resolvedIconName: {
        if (iconName && iconName.length > 0) return iconName
        return isDir ? "folder" : "text-x-generic"
    }

    Icon {
        id: themedIcon
        anchors.fill: parent
        name: root.resolvedIconName
        filled: root.isDir
        color: isDir
            ? (theme ? theme.accent : "#4A473E")
            : (theme ? theme.textSecondary : "#8A867E")
    }

    Rectangle {
        anchors.fill: parent
        visible: false // Icon component handles loading/fallback better or we can use a placeholder
        radius: Math.max(3, Math.round(width * 0.16))
        color: isDir
            ? (theme ? theme.accentSoft : "#C4B28A")
            : (theme ? theme.surfaceMuted : "#5E5E66")
        border.width: isDir ? 0 : 1
        border.color: theme ? theme.borderStrong : "#7B7B85"
    }
}
