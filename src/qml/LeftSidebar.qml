import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var theme
    property var placesModel
    property string activePath: ""
    property string homePath: ""

    signal pathActivated(string path)
    signal removeBookmarkRequested(string path)

    color: theme.sidebar

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.space12 : 12
        spacing: theme ? theme.space6 : 6

        ListView {
            id: sideList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: root.placesModel
            spacing: theme ? theme.space4 : 4
            clip: true
            section.property: "category"
            section.criteria: ViewSection.FullString

            section.delegate: Rectangle {
                required property string section
                width: sideList.width
                height: 24
                color: "transparent"

                readonly property string sectionTitle: {
                    if (section === "0") return "PLACES"
                    if (section === "1") return "BOOKMARKS"
                    if (section === "2") return "DEVICES"
                    if (section === "3") return "RECENT"
                    return "OTHER"
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: theme ? theme.space10 : 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: parent.sectionTitle
                    color: theme ? theme.textMuted : "#8A867E"
                    font.pixelSize: theme ? theme.fontLabel : 10
                    font.bold: true
                    font.letterSpacing: theme ? theme.letterSpacingLabel : 1.0
                }
            }

            delegate: SidebarItem {
                width: sideList.width
                text: model.name
                iconName: model.icon
                theme: root.theme
                active: root.activePath === model.path
                showAction: model.category === 1
                actionIconName: "close"
                actionToolTip: "Remove bookmark"
                onClicked: root.pathActivated(model.path)
                onActionClicked: root.removeBookmarkRequested(model.path)
            }
        }
    }

    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: theme.border
    }
}
