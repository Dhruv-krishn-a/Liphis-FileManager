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

    Image {
        id: themedIcon
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: "image://icon/" + root.resolvedIconName
        sourceSize.width: Math.max(64, Math.round(root.width * 3))
        sourceSize.height: Math.max(64, Math.round(root.height * 3))
        smooth: true
        mipmap: true
        antialiasing: true
    }

    Rectangle {
        anchors.fill: parent
        visible: themedIcon.status !== Image.Ready
        radius: Math.max(3, Math.round(width * 0.16))
        color: isDir
            ? (theme ? theme.accentSoft : "#C4B28A")
            : (theme ? theme.surfaceMuted : "#5E5E66")
        border.width: isDir ? 0 : 1
        border.color: theme ? theme.borderStrong : "#7B7B85"
    }
}
