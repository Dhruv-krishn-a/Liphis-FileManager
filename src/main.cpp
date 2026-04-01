#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLocalServer>
#include <QLocalSocket>
#include <QFileInfo>

#include "AppController.hpp"
#include "AnalysisController.hpp"
#include "DocumentIntelligenceController.hpp"
#include "TerminalManager.hpp"
#include "CommandManager.hpp"
#include "src/ThumbnailManager.hpp"
#include "ThumbnailImageProvider.hpp"
#include "SystemIconProvider.hpp"

#include <QImageReader>
#include <QWindow>
#include <QtQml>
#include <unistd.h>
#include <git2.h>

int main(int argc, char **argv)
{
    git_libgit2_init();

    qputenv("QSG_RENDER_LOOP", "basic");

    const QByteArray lang = qgetenv("LANG");
    const QByteArray lcAll = qgetenv("LC_ALL");
    const QByteArray lcCtype = qgetenv("LC_CTYPE");
    if (lang.contains("ISO8859-1") || lcAll.contains("ISO8859-1") || lcCtype.contains("ISO8859-1") || lang.contains("en_IN")) {
        qputenv("LANG", "en_US.UTF-8");
        qputenv("LC_ALL", "en_US.UTF-8");
        qputenv("LC_CTYPE", "en_US.UTF-8");
    }

    QGuiApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Liphis");
    QCoreApplication::setOrganizationDomain("liphis.local");
    QCoreApplication::setApplicationName("Liphis");
    
    // Memory Optimization: Increased limit to 512MB to handle large high-res photos.
    QImageReader::setAllocationLimit(512);

    // 2. Single Instance Check (Existing logic)
    const QString serverName = "liphis_app_singleton_" + QString::number(getuid());
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(120)) {
        if (argc > 1) {
            socket.write(argv[1]);
            socket.waitForBytesWritten(1000);
        }
        return 0;
    }

    QLocalServer server;
    server.listen(serverName);

    // 3. Setup Shared Resources (Global Singletons)
    bool isRoot = (getuid() == 0);
    ThumbnailManager thumbManager(&app);
    PlacesModel placesModel(&app);
    CommandManager commandManager(&app);
    SystemIconProvider* iconProvider = new SystemIconProvider();
    iconProvider->setParent(&app); // Avoid leak
    
    qmlRegisterType<AppController>("Liphis.Core", 1, 0, "AppController");
    qmlRegisterType<AnalysisController>("Liphis.Core", 1, 0, "AnalysisController");
    qmlRegisterType<DocumentIntelligenceController>("Liphis.Core", 1, 0, "DocumentIntelligenceController");
    qmlRegisterType<FileListModel>("Liphis.Core", 1, 0, "FileListModel");
    qmlRegisterType<FileTreeModel>("Liphis.Core", 1, 0, "FileTreeModel");
    qmlRegisterType<TerminalManager>("Liphis.Core", 1, 0, "TerminalManager");

    QQmlApplicationEngine engine;
    
    // Provide global instances to QML
    engine.rootContext()->setContextProperty("isRootAccount", isRoot);
    engine.rootContext()->setContextProperty("globalThumbnailManager", &thumbManager);
    engine.rootContext()->setContextProperty("globalPlacesModel", &placesModel);
    engine.rootContext()->setContextProperty("commandManager", &commandManager);
    
    engine.addImageProvider("thumbs", new ThumbnailImageProvider(&thumbManager));
    engine.addImageProvider("icon", iconProvider);

    engine.loadFromModule("liphis", "Main");
    if (engine.rootObjects().isEmpty()) return -1;

    // Handle new instances attempting to start
    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket* client = server.nextPendingConnection();
        if (client) {
            client->waitForReadyRead(1000);
            QString arg = client->readAll();
            
            // Tell the UI to open this path
            for (auto* obj : engine.rootObjects()) {
                if (obj->objectName() == "mainWindow") {
                    if (!arg.isEmpty()) {
                        QMetaObject::invokeMethod(obj, "addTab", Q_ARG(QVariant, arg));
                    }
                    if (auto* window = qobject_cast<QWindow*>(obj)) {
                        window->show();
                        window->raise();
                        window->requestActivate();
                    }
                }
            }
            client->disconnectFromServer();
        }
    });

    int ret = app.exec();
    git_libgit2_shutdown();
    return ret;
}
