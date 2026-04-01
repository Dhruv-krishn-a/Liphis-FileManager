import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Connect to Server"
    width: 400; height: 200
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel

    signal connectRequested(string url)

    ColumnLayout {
        anchors.fill: parent; spacing: 10
        Label { text: "Server Address (e.g., smb://..., sftp://...):" }
        TextField {
            id: urlField
            Layout.fillWidth: true
            placeholderText: "smb://server/share"
        }
    }

    onOpened: { urlField.text = ""; urlField.forceActiveFocus() }
    onAccepted: connectRequested(urlField.text)
}