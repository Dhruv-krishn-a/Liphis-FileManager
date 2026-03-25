import QtQuick

QtObject {
    id: theme

    property bool isDark: false

    // Core palette (matte, warm, neutral)
    property color bg: isDark ? "#1C1C1C" : "#F5F2EC"
    property color surface: isDark ? "#232323" : "#FBFAF7"
    property color surfaceRaised: isDark ? "#2A2A2A" : "#FFFFFF"
    property color surfaceMuted: isDark ? "#262626" : "#F3EFE8"
    property color sidebar: isDark ? "#202020" : "#F3EFE8"
    property color header: isDark ? "#1E1E1E" : "#F8F6F1"

    property color border: isDark ? "#343434" : "#DDD6CB"
    property color borderStrong: isDark ? "#3F3F3F" : "#CDC3B5"

    property color textPrimary: isDark ? "#F3F1ED" : "#2A2622"
    property color textSecondary: isDark ? "#B5B1AA" : "#6F685F"
    property color textMuted: isDark ? "#8A867E" : "#8B847A"

    property color accent: isDark ? "#D4A373" : "#B9783E"
    property color accentSoft: isDark ? "#E6C6A4" : "#D9B38C"

    property color success: isDark ? "#88A676" : "#627F50"
    property color warning: isDark ? "#C89C65" : "#A2743E"
    property color error: isDark ? "#D16E62" : "#B45B50"

    property color hover: isDark ? Qt.rgba(1, 1, 1, 0.04) : Qt.rgba(0, 0, 0, 0.04)
    property color selection: isDark ? Qt.rgba(212/255.0, 163/255.0, 115/255.0, 0.14)
                                     : Qt.rgba(185/255.0, 120/255.0, 62/255.0, 0.12)
    property color pressed: isDark ? Qt.rgba(1, 1, 1, 0.07) : Qt.rgba(0, 0, 0, 0.07)

    Behavior on bg { ColorAnimation { duration: 260 } }
    Behavior on surface { ColorAnimation { duration: 260 } }
    Behavior on surfaceRaised { ColorAnimation { duration: 260 } }
    Behavior on surfaceMuted { ColorAnimation { duration: 260 } }
    Behavior on sidebar { ColorAnimation { duration: 260 } }
    Behavior on header { ColorAnimation { duration: 260 } }
    Behavior on border { ColorAnimation { duration: 260 } }
    Behavior on borderStrong { ColorAnimation { duration: 260 } }
    Behavior on textPrimary { ColorAnimation { duration: 260 } }
    Behavior on textSecondary { ColorAnimation { duration: 260 } }
    Behavior on textMuted { ColorAnimation { duration: 260 } }
    Behavior on accent { ColorAnimation { duration: 260 } }
    Behavior on accentSoft { ColorAnimation { duration: 260 } }
    Behavior on hover { ColorAnimation { duration: 220 } }
    Behavior on selection { ColorAnimation { duration: 220 } }

    // Typography tokens
    property int fontLabel: 10
    property int fontBody: 12
    property int fontTitle: 17
    property real letterSpacingLabel: 1.0
    property real letterSpacingBrand: 2.0

    // Radius tokens
    property int rSm: 8
    property int rMd: 10
    property int rLg: 14

    // Spacing tokens
    property int space2: 2
    property int space4: 4
    property int space6: 6
    property int space8: 8
    property int space10: 10
    property int space12: 12
    property int space16: 16
    property int space20: 20
    property int space24: 24

    // Sizing tokens
    property int controlXs: 22
    property int controlSm: 30
    property int controlMd: 38
    property int iconSm: 14
    property int iconMd: 16
    property int iconLg: 18
    property int sidebarWidth: 248
    property int inspectorWidth: 316
    property int headerHeight: 62
    property int statusBarHeight: 32

    // Backward-compatible aliases for existing code
    property color surfaceMain: surface
    property color surfaceElevated: surfaceRaised
    property color surfaceSecondary: surfaceMuted
    property color sidebarBg: sidebar
    property color headerBg: header

    property int radius: rMd
    property int radiusLarge: rLg
    property int radiusSmall: rSm

    property int spacingSmall: space8
    property int spacingMedium: space16
    property int spacingLarge: space24
}
