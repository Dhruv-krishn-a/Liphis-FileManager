import QtQuick

QtObject {
    id: theme

    property bool isDark: true // Can be toggled if needed later

    // Colors based on the provided prompt
    property color bg: isDark ? "#1C1C1C" : "#F5F2EC"
    property color surfaceMain: isDark ? "#232323" : "#FBFAF7"
    property color surfaceElevated: isDark ? "#2A2A2A" : "#FFFFFF"
    property color surfaceSecondary: isDark ? "#2F2F2F" : "#F1EDE6"
    property color sidebarBg: isDark ? "#202020" : "#F3EFE8"
    property color headerBg: isDark ? "#1E1E1E" : "#F8F6F1"
    
    property color border: isDark ? "#343434" : "#DDD6CB"
    property color borderStrong: isDark ? "#404040" : "#CFC7BC"
    
    property color textPrimary: isDark ? "#F3F1ED" : "#2A2622"
    property color textSecondary: isDark ? "#B5B1AA" : "#6F685F"
    property color textMuted: isDark ? "#8A867E" : "#8B847A"
    
    property color accent: isDark ? "#D4A373" : "#B9783E"
    property color accentSoft: isDark ? "#E6C6A4" : "#D9B38C"
    
    property color success: isDark ? "#7FA36A" : "#5F8451"
    property color warning: isDark ? "#C8A15A" : "#A97B3D"
    property color error: isDark ? "#D66A5B" : "#B85A4E"
    
    property color selection: isDark ? Qt.rgba(0.831, 0.639, 0.451, 0.14) : Qt.rgba(0.725, 0.471, 0.243, 0.12)
    property color hover: isDark ? Qt.rgba(1, 1, 1, 0.04) : Qt.rgba(0, 0, 0, 0.04)

    // Common styling
    property int radius: 10
    property int radiusLarge: 14
    property int radiusSmall: 6

    property int spacingSmall: 8
    property int spacingMedium: 16
    property int spacingLarge: 24
}
