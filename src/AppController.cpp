#include "AppController.hpp"
#include "FileSystemEngine.hpp"
#include "ThreadPool.hpp"

#include <algorithm>
#include <cctype>
#include <QMetaObject>
#include <filesystem>
#include <QVariantMap>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QProcessEnvironment>
#include <QPointer>
#include <QTimer>
#include <unistd.h>

namespace fs = std::filesystem;

AppController::AppController(QObject *parent)
    : QObject(parent)
{
    m_proxyModel.setSourceModel(&m_fileModel);

    // File Watcher
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
        if (path == m_currentPath) {
            refresh();
        }
    });
}

void AppController::setThumbnailManager(ThumbnailManager* manager)
{
    if (m_thumbnailManager == manager) return;
    if (m_thumbnailManager) disconnect(m_thumbnailManager, nullptr, this, nullptr);
    m_thumbnailManager = manager;
    if (m_thumbnailManager) {
        connect(m_thumbnailManager, &ThumbnailManager::thumbnailReady, this, [this](const QString &filePath, const QString &thumbPath) {
            QMetaObject::invokeMethod(this, [this, filePath, thumbPath]() {
                m_fileModel.updateThumbnail(filePath, thumbPath);
            }, Qt::QueuedConnection);
        });
    }
}

void AppController::setPlacesModel(PlacesModel* model)
{
    if (m_placesModel == model) return;
    m_placesModel = model;
    emit placesModelChanged();
}

bool AppController::loading() const { return m_loading; }
QObject* AppController::fileModel() { return &m_proxyModel; }
QObject* AppController::placesModel() { return m_placesModel; }
QString AppController::currentPath() const { return m_currentPath; }
QString AppController::homePath() const { return QDir::homePath(); }
QString AppController::title() const {
    if (m_currentPath == "/") return "Root";
    QFileInfo fi(m_currentPath);
    QString name = fi.fileName();
    return name.isEmpty() ? "Root" : name;
}

bool AppController::canGoBack() const { return m_historyIndex > 0; }
bool AppController::canGoForward() const { return m_historyIndex < m_history.size() - 1; }
bool AppController::showHiddenFiles() const { return m_proxyModel.showHidden(); }
void AppController::setShowHiddenFiles(bool show) { m_proxyModel.setShowHidden(show); }
bool AppController::hasClipboard() const { return !m_clipboardPath.isEmpty(); }
QString AppController::clipboardPath() const { return m_clipboardPath; }
bool AppController::isCutOp() const { return m_isCutOp; }

int AppController::iconSize() const { return m_iconSize; }
void AppController::setIconSize(int size) {
    if (m_iconSize != size) {
        m_iconSize = size;
        emit iconSizeChanged();
    }
}

QString AppController::viewMode() const { return m_viewMode; }
void AppController::setViewMode(const QString &mode) {
    if (m_viewMode != mode) {
        m_viewMode = mode;
        emit viewModeChanged();
    }
}

QStringList AppController::availableExtensions() const {
    return m_fileModel.availableExtensions();
}

QVariantMap AppController::gitStatus() const {
    return m_gitStatus;
}

void AppController::updateGitStatus() {
    QString path = m_currentPath;
    if (path.isEmpty()) return;
    if (!QDir(path).exists(".git") && !QDir(path + "/..").exists(".git")) {
        m_gitStatus.clear();
        emit gitStatusChanged();
        return;
    }
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, path]() {
        if (!safeThis) return;
        QVariantMap statusMap;
        QProcess proc;
        proc.setWorkingDirectory(path);
        proc.start("git", QStringList() << "status" << "--porcelain");
        if (proc.waitForFinished(1000)) {
            QString output = proc.readAllStandardOutput();
            QStringList lines = output.split('\n', Qt::SkipEmptyParts);
            for (const QString& line : lines) {
                if (line.length() > 3) {
                    QString state = line.left(2).trimmed();
                    QString file = line.mid(3);
                    statusMap[file] = state;
                }
            }
        }
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, statusMap]() {
                if (safeThis) {
                    safeThis->m_gitStatus = statusMap;
                    emit safeThis->gitStatusChanged();
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::openPath(const QString &path)
{
    if (path.isEmpty() || path == m_currentPath) return;
    QFileInfo fi(path);
    if (!fi.isDir()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        return;
    }
    if (m_historyIndex < m_history.size() - 1) m_history.resize(m_historyIndex + 1);
    m_history.append(path);
    m_historyIndex++;
    emit canGoBackChanged();
    emit canGoForwardChanged();
    loadPathInternal(path);
}

void AppController::loadPathInternal(const QString &path)
{
    const std::uint64_t generation = ++m_operationGeneration;
    m_loading = true;
    emit loadingChanged();
    m_cancelRequested = true;
    if (!m_currentPath.isEmpty()) m_watcher.removePath(m_currentPath);
    m_currentPath = path;
    if (!path.isEmpty()) m_watcher.addPath(path);
    emit currentPathChanged();
    emit titleChanged();
    if (m_placesModel) m_placesModel->addRecent(path);

    clearSelection();
    m_fileModel.clear();
    QPointer<AppController> safeThis(this);
    m_cancelRequested = false;
    auto shouldCancel = [safeThis, generation]() -> bool {
        return !safeThis || safeThis->m_cancelRequested.load() || safeThis->m_operationGeneration.load() != generation;
    };

    ThreadPool::instance().submit([safeThis, path, shouldCancel, generation]() {
        if (!safeThis) return;
        FileSystemEngine::listDirectoryStream(path.toStdString(), 50, [safeThis, generation](std::vector<FileMeta>&& batch) {
            if (safeThis) {
                QMetaObject::invokeMethod(safeThis, [safeThis, generation, b = std::move(batch)]() mutable {
                    if (!safeThis || safeThis->m_operationGeneration.load() != generation) return;
                    safeThis->m_fileModel.insertBatch(std::move(b));
                }, Qt::QueuedConnection);
            }
        }, shouldCancel);
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, generation]() {
                if (safeThis) {
                    if (safeThis->m_operationGeneration.load() != generation) return;
                    safeThis->m_loading = false;
                    emit safeThis->loadingChanged();
                    emit safeThis->availableExtensionsChanged();
                    safeThis->updateGitStatus();
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::goUp() {
    if (m_currentPath.isEmpty()) return;
    fs::path p(m_currentPath.toStdString());
    if (p.has_parent_path()) openPath(QString::fromStdString(p.parent_path().string()));
}

void AppController::goBack() {
    if (m_historyIndex > 0) {
        m_historyIndex--;
        emit canGoBackChanged();
        emit canGoForwardChanged();
        loadPathInternal(m_history[m_historyIndex]);
    }
}

void AppController::goForward() {
    if (m_historyIndex < m_history.size() - 1) {
        m_historyIndex++;
        emit canGoBackChanged();
        emit canGoForwardChanged();
        loadPathInternal(m_history[m_historyIndex]);
    }
}

void AppController::refresh() { loadPathInternal(m_currentPath); }
void AppController::setSearchText(const QString &text) { m_proxyModel.setFilterFixedString(text); }

void AppController::startGlobalSearch(const QString &pattern)
{
    if (pattern.isEmpty()) return;
    const std::uint64_t generation = ++m_operationGeneration;
    m_loading = true;
    emit loadingChanged();
    m_fileModel.clear();
    m_proxyModel.setFilterFixedString(""); // Clear proxy filter during global search
    m_cancelRequested = true;
    QPointer<AppController> safeThis(this);
    m_cancelRequested = false;
    auto shouldCancel = [safeThis, generation]() -> bool {
        return !safeThis || safeThis->m_cancelRequested.load() || safeThis->m_operationGeneration.load() != generation;
    };
    ThreadPool::instance().submit([safeThis, path = m_currentPath, pattern, shouldCancel, generation]() {
        FileSystemEngine::searchRecursive(path.toStdString(), pattern.toStdString(), [safeThis, generation](std::vector<FileMeta>&& batch) {
            if (safeThis) {
                QMetaObject::invokeMethod(safeThis, [safeThis, generation, b = std::move(batch)]() mutable {
                    if (!safeThis || safeThis->m_operationGeneration.load() != generation) return;
                    safeThis->m_fileModel.insertBatch(std::move(b));
                }, Qt::QueuedConnection);
            }
        }, shouldCancel);
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, generation]() {
                if (safeThis) {
                    if (safeThis->m_operationGeneration.load() != generation) return;
                    safeThis->m_loading = false;
                    emit safeThis->loadingChanged();
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::createFolder(const QString &name)
{
    if (name.isEmpty() || m_currentPath.isEmpty()) return;
    QString fullPath = m_currentPath + "/" + name;
    auto res = FileSystemEngine::createDirectory(fullPath.toStdString());
    if (res.success) { refresh(); emit operationSuccess("Folder created"); }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::renameItem(const QString &oldPath, const QString &newName)
{
    if (oldPath.isEmpty() || newName.isEmpty()) return;
    QFileInfo fi(oldPath);
    QString newPath = fi.absolutePath() + "/" + newName;
    auto res = FileSystemEngine::renamePath(oldPath.toStdString(), newPath.toStdString());
    if (res.success) { refresh(); emit operationSuccess("Renamed"); }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::bulkRename(const QStringList &paths, const QString &prefix, const QString &suffix, const QString &find, const QString &replace)
{
    for (const QString &path : paths) {
        QFileInfo fi(path);
        QString name = fi.fileName();
        if (!find.isEmpty()) name.replace(find, replace);
        name = prefix + name + suffix;
        renameItem(path, name);
    }
}

void AppController::deleteItem(const QString &path)
{
    auto res = FileSystemEngine::deletePath(path.toStdString());
    if (res.success) { refresh(); emit operationSuccess("Deleted"); }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::copyItem(const QString &path)
{
    m_clipboardPath = path; m_isCutOp = false;
    emit clipboardChanged();
    if (!path.isEmpty()) emit operationSuccess("Copied to clipboard");
}

void AppController::cutItem(const QString &path)
{
    m_clipboardPath = path; m_isCutOp = true;
    emit clipboardChanged();
    emit operationSuccess("Cut to clipboard");
}

void AppController::pasteItem()
{
    if (m_clipboardPath.isEmpty() || m_currentPath.isEmpty()) return;
    QString src = m_clipboardPath; QString destDir = m_currentPath; bool isCut = m_isCutOp;
    emit operationProgress(0.0);
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, src, destDir, isCut]() {
        fs::path s(src.toStdString()); fs::path d = fs::path(destDir.toStdString()) / s.filename();
        FileSystemEngine::OperationResult res;
        if (isCut) res = FileSystemEngine::movePath(src.toStdString(), d.string());
        else res = FileSystemEngine::copyPath(src.toStdString(), d.string());
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, isCut, res]() { 
                if (safeThis) {
                    emit safeThis->operationProgress(1.0);
                    if (res.success) {
                        safeThis->refresh();
                        if (isCut) { safeThis->m_clipboardPath = ""; emit safeThis->clipboardChanged(); }
                        emit safeThis->operationSuccess("Paste successful");
                    } else emit safeThis->operationError(QString::fromStdString(res.message));
                }
            });
        }
    });
}

void AppController::requestThumbnail(const QString &path) { if (m_thumbnailManager) m_thumbnailManager->requestThumbnail(path); }

void AppController::selectPath(const QString &path)
{
    if (m_selectedPath == path && m_selectedPaths.size() == 1 && m_selectedPaths.contains(path)) return;
    m_selectedPath = path; m_selectedPaths.clear();
    if (!path.isEmpty()) m_selectedPaths.append(path);
    emit selectedPathChanged(); emit selectedPathsChanged();
}

void AppController::toggleSelection(const QString &path)
{
    if (m_selectedPaths.contains(path)) m_selectedPaths.removeAll(path);
    else m_selectedPaths.append(path);
    if (m_selectedPaths.isEmpty()) m_selectedPath = "";
    else m_selectedPath = m_selectedPaths.last();
    emit selectedPathChanged(); emit selectedPathsChanged();
}

void AppController::clearSelection()
{
    if (m_selectedPaths.isEmpty()) return;
    m_selectedPath = ""; m_selectedPaths.clear();
    emit selectedPathChanged(); emit selectedPathsChanged();
}

void AppController::selectAll()
{
    m_selectedPaths.clear();
    int rowCount = m_proxyModel.rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex proxyIdx = m_proxyModel.index(i, 0);
        QString path = m_proxyModel.data(proxyIdx, FileListModel::PathRole).toString();
        if (!path.isEmpty()) m_selectedPaths.append(path);
    }
    if (!m_selectedPaths.isEmpty()) m_selectedPath = m_selectedPaths.last();
    else m_selectedPath = "";
    emit selectedPathChanged();
    emit selectedPathsChanged();
}

void AppController::startRename(const QString &path) { emit renameRequested(path); }
QString AppController::selectedPath() const { return m_selectedPath; }
QStringList AppController::selectedPaths() const { return m_selectedPaths; }
bool AppController::hasSelection() const { return !m_selectedPaths.isEmpty(); }
QVariantMap AppController::metadataForPath(const QString &path) const { return m_fileModel.metadataForPath(path); }

void AppController::requestThumbnailsForRange(int firstIndex, int lastIndex)
{
    int rowCount = m_proxyModel.rowCount(); if (rowCount == 0) return;
    firstIndex = qMax(0, firstIndex); lastIndex = qMin(lastIndex, rowCount - 1);
    for (int i = firstIndex; i <= lastIndex; ++i) {
        QModelIndex proxyIdx = m_proxyModel.index(i, 0);
        QString path = m_proxyModel.data(proxyIdx, FileListModel::PathRole).toString();
        if (!path.isEmpty() && m_thumbnailManager) m_thumbnailManager->requestThumbnail(path);
    }
}

QString AppController::getFilePreview(const QString &path)
{
    QFile file(path); if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "";
    QTextStream in(&file); return in.read(1000);
}

void AppController::openInTerminal(const QString &path)
{
    QFileInfo fi(path); QString dir = fi.isDir() ? fi.absoluteFilePath() : fi.absolutePath();
    QString envTerm = qgetenv("TERMINAL");
    if (!envTerm.isEmpty()) { if (QProcess::startDetached(envTerm, {dir})) return; }
    const QStringList terminals = { "konsole", "--workdir", "gnome-terminal", "--working-directory=", "xfce4-terminal", "--working-directory=", "terminator", "--working-directory=", "alacritty", "--working-directory=", "kitty", "--working-directory=", "xterm", "-cd" };
    for (int i = 0; i < terminals.size(); i += 2) {
        QString cmd = terminals[i]; QString arg = terminals[i+1]; QString fullArg = (arg.endsWith("=") ? arg + dir : arg); QStringList args;
        if (!arg.isEmpty()) { if (arg == "-cd" || arg == "--workdir") args << arg << dir; else args << fullArg; }
        if (QProcess::startDetached(cmd, args)) return;
    }
    emit operationError("No supported terminal found");
}

void AppController::copyToClipboard(const QString &text) { QGuiApplication::clipboard()->setText(text); emit operationSuccess("Copied path"); }

void AppController::trashItems(const QStringList &paths)
{
    if (paths.isEmpty()) return;
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, paths]() {
        for (const QString &path : paths) QProcess::execute("gio", {"trash", path});
        if (safeThis) { QMetaObject::invokeMethod(safeThis, [safeThis]() { if (safeThis) { safeThis->refresh(); emit safeThis->operationSuccess("Moved to Trash"); } }, Qt::QueuedConnection); }
    });
}

void AppController::analyseFolder(const QString &path) { emit analyseRequested(path); }

void AppController::compressItems(const QStringList &paths)
{
    if (paths.isEmpty()) return;
    QString parentDir = QFileInfo(paths.first()).absolutePath();
    QString archiveName = parentDir + "/archive.tar.gz";
    QStringList args; args << "-czf" << archiveName;
    for(const auto& p : paths) args << QFileInfo(p).fileName();
    if (QProcess::startDetached("tar", args, parentDir)) emit operationSuccess("Compressing...");
    else emit operationError("Failed to start tar");
}

void AppController::extractItem(const QString &path)
{
    if (path.isEmpty()) return;
    QFileInfo fi(path); QString dir = fi.absolutePath(); QString ext = fi.suffix().toLower();
    QString cmd; QStringList args;
    if (ext == "zip") { cmd = "unzip"; args << path << "-d" << dir; }
    else if (ext == "tar" || ext == "gz" || ext == "bz2" || ext == "xz") { cmd = "tar"; args << "-xf" << path << "-C" << dir; }
    else { emit operationError("Unsupported archive format"); return; }
    if (QProcess::startDetached(cmd, args, dir)) emit operationSuccess("Extracting...");
    else emit operationError("Failed to start " + cmd);
}

void AppController::openInCode(const QString &path) { if (!QProcess::startDetached("code", {path})) emit operationError("VS Code not found"); }

void AppController::addToBookmarks(const QString &path, const QString &name) { if (m_placesModel) m_placesModel->addBookmark(path, name); emit operationSuccess("Bookmark added"); }
void AppController::removeFromBookmarks(int index) { if (m_placesModel) m_placesModel->removeBookmark(index); }

void AppController::openAsRoot(const QString &path)
{
    if (getuid() == 0) { emit operationError("Already running as root"); return; }
    static bool isElevating = false; if (isElevating) return; isElevating = true;
    QString self = QCoreApplication::applicationFilePath();
    if (!QProcess::startDetached("pkexec", {self, path})) { emit operationError("Failed to launch as root"); isElevating = false; }
    else { emit operationSuccess("Launching elevated instance..."); QTimer::singleShot(5000, []() { isElevating = false; }); }
}

QString AppController::computeChecksum(const QString &path)
{
    QFile f(path); if (f.open(QFile::ReadOnly)) { QCryptographicHash hash(QCryptographicHash::Sha256); if (hash.addData(&f)) return hash.result().toHex(); }
    emit operationError("Failed to read file"); return "Error";
}

void AppController::duplicateItem(const QString &path)
{
    if (path.isEmpty()) return;
    QFileInfo fi(path); QString newPath = fi.absolutePath() + "/" + fi.baseName() + "_copy"; if (!fi.suffix().isEmpty()) newPath += "." + fi.suffix();
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, path, newPath]() {
        auto res = FileSystemEngine::copyPath(path.toStdString(), newPath.toStdString());
        if (safeThis) { QMetaObject::invokeMethod(safeThis, [safeThis, res]() { if (safeThis) { if (res.success) { safeThis->refresh(); emit safeThis->operationSuccess("Duplicated"); } else emit safeThis->operationError(QString::fromStdString(res.message)); } }, Qt::QueuedConnection); }
    });
}

void AppController::createSymlink(const QString &target, const QString &linkName)
{
    QString linkPath = m_currentPath + "/" + linkName;
    auto res = FileSystemEngine::createSymlink(target.toStdString(), linkPath.toStdString());
    if (res.success) { refresh(); emit operationSuccess("Link created"); }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::makeExecutable(const QString &path)
{
    auto res = FileSystemEngine::setPermissions(path.toStdString(), 0755);
    if (res.success) { refresh(); emit operationSuccess("Made executable"); }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::setPermissions(const QString &path, const QString &mode)
{
    bool ok; int numericMode = mode.toInt(&ok, 8); if (!ok) { emit operationError("Invalid permission mode"); return; }
    auto res = FileSystemEngine::setPermissions(path.toStdString(), numericMode);
    if (res.success) { refresh(); emit operationSuccess("Permissions updated"); }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::setWallpaper(const QString &path)
{
    QString desktop = QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP").toLower();
    if (desktop.contains("gnome") || desktop.contains("unity")) { 
        QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri", "file://" + path}); 
        QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri-dark", "file://" + path}); 
    }
    else if (desktop.contains("kde") || desktop.contains("plasma")) {
        QString script = QString("var allDesktops = desktops();"
                                 "for (i=0;i<allDesktops.length;i++) {"
                                 "    d = allDesktops[i];"
                                 "    d.wallpaperPlugin = \"org.kde.image\";"
                                 "    d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");"
                                 "    d.writeConfig(\"Image\", \"file://%1\");"
                                 "}").arg(path);
        QProcess::startDetached("qdbus", {"org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell.evaluateScript", script});
    }
    else { if (QProcess::startDetached("feh", {"--bg-scale", path})) emit operationSuccess("Wallpaper set (feh)"); else emit operationError("Could not set wallpaper (try installing feh)"); }
}

void AppController::mountRemote(const QString &url)
{
    if (QProcess::startDetached("gio", {"mount", url})) emit operationSuccess("Mounting " + url + "...");
    else emit operationError("Failed to mount remote");
}
