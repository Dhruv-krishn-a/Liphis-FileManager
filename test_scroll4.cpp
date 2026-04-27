#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStyleHints>
#include <QDebug>

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    app.styleHints()->setWheelScrollLines(3);
    qDebug() << "Wheel scroll lines:" << app.styleHints()->wheelScrollLines();
    QQmlApplicationEngine engine;
    engine.load("test_scroll4.qml");
    return app.exec();
}
