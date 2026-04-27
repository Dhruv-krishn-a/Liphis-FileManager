#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStyleHints>
#include <QDebug>

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    app.styleHints()->setWheelScrollLines(100);
    qDebug() << "Wheel scroll lines:" << app.styleHints()->wheelScrollLines();
    QQmlApplicationEngine engine;
    engine.load("test_scroll_hints.qml");
    return app.exec();
}
