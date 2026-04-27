#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStyleHints>

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    app.styleHints()->setWheelScrollLines(100);
    QQmlApplicationEngine engine;
    engine.loadFromData(R"(
import QtQuick
import QtQuick.Controls
import QtQuick.Window

ApplicationWindow {
    width: 400
    height: 400
    visible: true

    ListView {
        anchors.fill: parent
        model: 1000
        delegate: Text {
            text: "Item " + modelData
            font.pixelSize: 20
            height: 40
        }
    }
}
)");
    return app.exec();
}
