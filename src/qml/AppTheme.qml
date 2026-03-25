import QtQuick

QtObject {
    id: theme

    property bool isDark: false

    // Claude-like dual palette
    property color bg: isDark ? "#1B1917" : "#F7F6F3"
    property color surfaceMain: isDark ? "#23201D" : "#FCFBF9"
    property color surfaceElevated: isDark ? "#2B2723" : "#FFFFFF"
    property color surfaceSecondary: isDark ? "#34302B" : "#F3F1ED"
    property color sidebarBg: isDark ? "#211E1A" : "#F2F0EC"
    property color headerBg: isDark ? "#25211D" : "#FBFAF8"
    
    property color border: isDark ? "#3E3832" : "#E3DED6"
    property color borderStrong: isDark ? "#4B443D" : "#D2CBC1"
    
    property color textPrimary: isDark ? "#F1ECE4" : "#2F2A24"
    property color textSecondary: isDark ? "#B9B1A7" : "#645D53"
    property color textMuted: isDark ? "#978C7F" : "#7D7469"
    
    property color accent: isDark ? "#D29A64" : "#B87534"
    property color accentSoft: isDark ? "#E6BD8F" : "#D7A26B"
    
    property color success: isDark ? "#7FA36A" : "#5F8451"
    property color warning: isDark ? "#C8A15A" : "#A97B3D"
    property color error: isDark ? "#D66A5B" : "#B85A4E"
    
    property color selection: isDark ? Qt.rgba(0.824, 0.604, 0.392, 0.19) : Qt.rgba(0.721, 0.459, 0.204, 0.14)
    property color hover: isDark ? Qt.rgba(1, 1, 1, 0.06) : Qt.rgba(0.12, 0.09, 0.05, 0.05)

    // Common styling
    property int radius: 10
    property int radiusLarge: 14
    property int radiusSmall: 8

    property int spacingSmall: 8
    property int spacingMedium: 16
    property int spacingLarge: 24
}
