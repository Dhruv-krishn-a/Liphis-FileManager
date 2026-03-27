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
#include <QSettings>
#include <QDateTime>
#include <QFileInfo>
#include <QSet>
#include <QRegularExpression>
#include <QStorageInfo>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {
struct SearchQuery {
    QString term;
    QString extension;
    QString type; // all | file | folder
    QString kind; // all | image | video | audio | document | code | archive | folder
    qlonglong minSize{-1};
    qlonglong maxSize{-1};
    qlonglong minDate{-1};
    qlonglong maxDate{-1};
    bool showHidden{false};
    bool exact{false};
};

qlonglong parseSizeBytes(const QString &value) {
    QString s = value.trimmed().toLower();
    if (s.isEmpty()) return -1;
    qlonglong factor = 1;
    if (s.endsWith("kb")) { factor = 1024LL; s.chop(2); }
    else if (s.endsWith("mb")) { factor = 1024LL * 1024LL; s.chop(2); }
    else if (s.endsWith("gb")) { factor = 1024LL * 1024LL * 1024LL; s.chop(2); }
    else if (s.endsWith("tb")) { factor = 1024LL * 1024LL * 1024LL * 1024LL; s.chop(2); }
    else if (s.endsWith("b")) { factor = 1; s.chop(1); }
    bool ok = false;
    const double v = s.toDouble(&ok);
    if (!ok || v < 0.0) return -1;
    return static_cast<qlonglong>(v * static_cast<double>(factor));
}

qlonglong parseDaysToEpoch(const QString &value) {
    QString s = value.trimmed().toLower();
    if (s.endsWith("d")) s.chop(1);
    bool ok = false;
    const int days = s.toInt(&ok);
    if (!ok || days < 0) return -1;
    return QDateTime::currentSecsSinceEpoch() - static_cast<qlonglong>(days) * 24LL * 60LL * 60LL;
}

SearchQuery parseSearchQuery(const QString &raw, const bool defaultShowHidden) {
    SearchQuery q;
    q.type = "all";
    q.kind = "all";
    q.showHidden = defaultShowHidden;
    const QStringList tokens = raw.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    QStringList termTokens;
    for (const QString &token : tokens) {
        const int sep = token.indexOf(':');
        if (sep <= 0) {
            termTokens << token;
            continue;
        }
        const QString key = token.left(sep).trimmed().toLower();
        const QString value = token.mid(sep + 1).trimmed();

        if (key == "ext" || key == "extension") {
            QString ext = value.toLower();
            if (ext.startsWith(".")) ext.remove(0, 1);
            q.extension = ext;
            continue;
        }
        if (key == "type") {
            const QString v = value.toLower();
            if (v == "file" || v == "files") q.type = "file";
            else if (v == "folder" || v == "dir" || v == "directory") q.type = "folder";
            continue;
        }
        if (key == "kind") {
            const QString v = value.toLower();
            if (v == "image" || v == "video" || v == "audio" || v == "document"
                || v == "code" || v == "archive" || v == "folder") q.kind = v;
            else q.kind = "all";
            continue;
        }
        if (key == "hidden") {
            const QString v = value.toLower();
            q.showHidden = (v == "1" || v == "true" || v == "yes" || v == "on");
            continue;
        }
        if (key == "exact") {
            const QString v = value.toLower();
            q.exact = (v == "1" || v == "true" || v == "yes" || v == "on");
            continue;
        }
        if (key == "size") {
            QString v = value.trimmed().toLower();
            if (v.startsWith(">=")) q.minSize = parseSizeBytes(v.mid(2));
            else if (v.startsWith("<=")) q.maxSize = parseSizeBytes(v.mid(2));
            else if (v.startsWith(">")) q.minSize = parseSizeBytes(v.mid(1)) + 1;
            else if (v.startsWith("<")) q.maxSize = parseSizeBytes(v.mid(1)) - 1;
            else {
                const int dash = v.indexOf('-');
                if (dash > 0) {
                    q.minSize = parseSizeBytes(v.left(dash));
                    q.maxSize = parseSizeBytes(v.mid(dash + 1));
                } else {
                    const qlonglong exactSize = parseSizeBytes(v);
                    q.minSize = exactSize;
                    q.maxSize = exactSize;
                }
            }
            continue;
        }
        if (key == "modified") {
            QString v = value.trimmed().toLower();
            if (v.startsWith(">")) q.maxDate = parseDaysToEpoch(v.mid(1)); // older than N days
            else {
                const qlonglong cutoff = parseDaysToEpoch(v);
                if (cutoff >= 0) q.minDate = cutoff; // within last N days
            }
            continue;
        }

        termTokens << token;
    }
    q.term = termTokens.join(' ').trimmed();
    if (raw.contains(" exact ", Qt::CaseInsensitive) || raw.startsWith("exact ", Qt::CaseInsensitive)) q.exact = true;
    return q;
}

bool matchesStructuredFilters(const FileMeta &m, const SearchQuery &q) {
    const QString name = QString::fromStdString(m.name);
    const QString lowerName = name.toLower();
    if (!q.showHidden && lowerName.startsWith(".")) return false;
    if (q.type == "file" && m.isDir) return false;
    if (q.type == "folder" && !m.isDir) return false;
    if (q.kind == "folder" && !m.isDir) return false;
    if (q.kind != "all" && q.kind != "folder") {
        if (m.isDir) return false;
        QString ext;
        const int dot = lowerName.lastIndexOf('.');
        if (dot >= 0 && dot < lowerName.length() - 1) ext = lowerName.mid(dot + 1);
        const QStringList image = {"png","jpg","jpeg","webp","gif","bmp","svg","avif","heic"};
        const QStringList video = {"mp4","mkv","avi","mov","webm","flv","wmv","m4v"};
        const QStringList audio = {"mp3","wav","flac","ogg","m4a","aac"};
        const QStringList document = {"pdf","doc","docx","xls","xlsx","ppt","pptx","txt","odt","ods","odp","rtf"};
        const QStringList code = {"cpp","c","h","hpp","py","js","ts","tsx","jsx","java","rs","go","sh","json","yaml","yml","toml","md","html","css","php"};
        const QStringList archive = {"zip","tar","gz","bz2","xz","7z","rar","tgz"};
        if (q.kind == "image" && !image.contains(ext)) return false;
        if (q.kind == "video" && !video.contains(ext)) return false;
        if (q.kind == "audio" && !audio.contains(ext)) return false;
        if (q.kind == "document" && !document.contains(ext)) return false;
        if (q.kind == "code" && !code.contains(ext)) return false;
        if (q.kind == "archive" && !archive.contains(ext)) return false;
    }

    if (!q.extension.isEmpty()) {
        if (m.isDir) return false;
        if (q.extension == "no extension") {
            if (lowerName.contains(".")) return false;
        } else if (!lowerName.endsWith("." + q.extension.toLower())) {
            return false;
        }
    }

    if (!m.isDir) {
        if (q.minSize >= 0 && static_cast<qlonglong>(m.size) < q.minSize) return false;
        if (q.maxSize >= 0 && static_cast<qlonglong>(m.size) > q.maxSize) return false;
    }
    if (q.minDate >= 0 && static_cast<qlonglong>(m.mtime) < q.minDate) return false;
    if (q.maxDate >= 0 && static_cast<qlonglong>(m.mtime) > q.maxDate) return false;

    if (!q.term.isEmpty()) {
        const QString term = q.term.toLower();
        const bool looksLikeFullFilename = !q.exact
            && term.contains('.')
            && !term.contains(' ')
            && !term.contains('*')
            && !term.contains('?');
        if (q.exact || looksLikeFullFilename) {
            if (lowerName != term) return false;
        } else if (!lowerName.contains(term)) {
            return false;
        }
    }
    return true;
}

bool hasCommand(const QString &name)
{
    QProcess p;
    p.start("sh", {"-lc", "command -v " + name});
    if (!p.waitForFinished(500)) return false;
    return p.exitCode() == 0 && !p.readAllStandardOutput().trimmed().isEmpty();
}

bool buildMeta(const QString &rawPath, bool isDirHint, FileMeta &m)
{
    QFileInfo fi(rawPath);
    if (!fi.exists()) return false;

    m.path = fi.absoluteFilePath().toStdString();
    m.name = fi.fileName().toStdString();
    m.isDir = isDirHint || fi.isDir();
    m.size = m.isDir ? 0 : static_cast<std::uint64_t>(fi.size());
    m.mtime = static_cast<std::uint64_t>(fi.lastModified().toSecsSinceEpoch());
    m.ctime = m.mtime;
    m.atime = m.mtime;
    return true;
}

bool runFdSearch(const QString &rootPath,
                 const SearchQuery &parsed,
                 const std::function<bool()> &shouldCancel,
                 const std::function<void(std::vector<FileMeta>&&)> &onBatch)
{
    static const bool fdAvailable = hasCommand("fd");
    if (!fdAvailable) return false;

    QStringList types;
    if (parsed.kind == "folder") {
        types << "d";
    } else if (!parsed.extension.isEmpty()) {
        if (parsed.type == "folder") return true;
        types << "f";
    } else if (parsed.type == "file") {
        types << "f";
    } else if (parsed.type == "folder") {
        types << "d";
    } else {
        types << "f" << "d";
    }

    const QString termPattern = parsed.term.isEmpty() ? "." : parsed.term;
    for (const QString &type : types) {
        if (shouldCancel()) return true;
        QProcess proc;
        QStringList args;
        args << "--absolute-path" << "--color" << "never" << "--type" << type << "--hidden" << "--no-ignore";
        if (parsed.exact) args << "--fixed-strings";
        if (!parsed.extension.isEmpty() && parsed.extension != "no extension" && type == "f") {
            args << "-e" << parsed.extension;
        }
        args << termPattern << rootPath;
        proc.start("fd", args);
        if (!proc.waitForStarted(1000)) return false;

        std::vector<FileMeta> batch;
        batch.reserve(64);
        QByteArray pending;

        auto flushLines = [&]() {
            int lineEnd = pending.indexOf('\n');
            while (lineEnd >= 0) {
                const QByteArray line = pending.left(lineEnd).trimmed();
                pending.remove(0, lineEnd + 1);
                if (!line.isEmpty()) {
                    FileMeta meta;
                    if (buildMeta(QString::fromUtf8(line), type == "d", meta) && matchesStructuredFilters(meta, parsed)) {
                        batch.push_back(std::move(meta));
                        if (batch.size() >= 64) {
                            onBatch(std::move(batch));
                            batch.clear();
                            batch.reserve(64);
                        }
                    }
                }
                lineEnd = pending.indexOf('\n');
            }
        };

        while (proc.state() != QProcess::NotRunning) {
            if (shouldCancel()) {
                proc.kill();
                proc.waitForFinished(1000);
                return true;
            }
            proc.waitForReadyRead(80);
            pending += proc.readAllStandardOutput();
            flushLines();
        }
        pending += proc.readAllStandardOutput();
        flushLines();
        if (!pending.trimmed().isEmpty()) {
            FileMeta meta;
            if (buildMeta(QString::fromUtf8(pending.trimmed()), type == "d", meta) && matchesStructuredFilters(meta, parsed)) {
                batch.push_back(std::move(meta));
            }
        }
        if (!batch.empty()) {
            onBatch(std::move(batch));
        }
    }
    return true;
}

QStringList prunedRoots(QStringList roots)
{
    roots.removeAll("");
    roots.removeDuplicates();
    std::sort(roots.begin(), roots.end(), [](const QString &a, const QString &b) {
        return a.length() < b.length();
    });
    QStringList pruned;
    for (const QString &r : roots) {
        bool nested = false;
        const QString normalized = QDir::cleanPath(r);
        for (const QString &k : pruned) {
            const QString keep = QDir::cleanPath(k);
            if (normalized == keep) { nested = true; break; }
            if (normalized.startsWith(keep + "/")) { nested = true; break; }
        }
        if (!nested) pruned << normalized;
    }
    return pruned;
}
}

AppController::AppController(QObject *parent)
    : QObject(parent)
{
    m_proxyModel.setSourceModel(&m_fileModel);
    loadRecentSearches();

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
    if (path.isEmpty()) return;

    QString resolvedPath = path.trimmed();
    const QUrl asUrl(resolvedPath);
    if (asUrl.isValid() && asUrl.isLocalFile()) {
        resolvedPath = asUrl.toLocalFile();
    } else if (resolvedPath.startsWith("file://")) {
        resolvedPath.remove(0, QString("file://").size());
    }

    if (resolvedPath.isEmpty() || resolvedPath == m_currentPath) return;

    QFileInfo fi(resolvedPath);
    if (!fi.isDir()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(resolvedPath));
        return;
    }

    if (m_historyIndex < m_history.size() - 1) m_history.resize(m_historyIndex + 1);
    m_history.append(resolvedPath);
    m_historyIndex++;
    emit canGoBackChanged();
    emit canGoForwardChanged();
    loadPathInternal(resolvedPath);
}

void AppController::loadPathInternal(const QString &path)
{
    const std::uint64_t generation = ++m_operationGeneration;
    m_loading = true;
    m_searchInProgress = false;
    m_activeSearchTerm = "";
    m_proxyModel.setSearchQuery("");
    m_proxyModel.setExtensionFilter("");
    m_proxyModel.setTypeFilter("all");
    m_proxyModel.setMinSize(-1);
    m_proxyModel.setMaxSize(-1);
    m_proxyModel.setMinDate(-1);
    m_proxyModel.setMaxDate(-1);
    m_searchMode = "local";
    emit searchModeChanged();
    emit activeSearchTermChanged();
    emit searchInProgressChanged();
    emit requestSearchClear();
    emit loadingChanged();
    m_cancelRequested = true;
    if (!m_currentPath.isEmpty()) m_watcher.removePath(m_currentPath);
    m_currentPath = path;
    m_globalSearchActive = false;
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
void AppController::setSearchText(const QString &text) {
    const SearchQuery parsed = parseSearchQuery(text, m_proxyModel.showHidden());
    if (m_activeSearchTerm != parsed.term) {
        m_activeSearchTerm = parsed.term;
        emit activeSearchTermChanged();
    }
    m_proxyModel.setSearchQuery(parsed.term);
    m_proxyModel.setExtensionFilter(parsed.extension);
    m_proxyModel.setTypeFilter(parsed.type);
    m_proxyModel.setMinSize(parsed.minSize);
    m_proxyModel.setMaxSize(parsed.maxSize);
    m_proxyModel.setMinDate(parsed.minDate);
    m_proxyModel.setMaxDate(parsed.maxDate);
    m_proxyModel.setShowHidden(parsed.showHidden);
    m_proxyModel.setExactMatch(parsed.exact);
}

void AppController::startGlobalSearch(const QString &pattern)
{
    const SearchQuery parsed = parseSearchQuery(pattern, m_proxyModel.showHidden());
    if (pattern.trimmed().isEmpty() && parsed.extension.isEmpty()
        && parsed.type == "all" && parsed.minSize < 0 && parsed.maxSize < 0
        && parsed.minDate < 0 && parsed.maxDate < 0) return;

    const std::uint64_t generation = ++m_operationGeneration;
    m_loading = true;
    m_searchInProgress = true;
    m_globalSearchActive = true;
    emit loadingChanged();
    emit searchInProgressChanged();
    if (m_activeSearchTerm != parsed.term) {
        m_activeSearchTerm = parsed.term;
        emit activeSearchTermChanged();
    }
    m_fileModel.clear();
    m_proxyModel.setSearchQuery(parsed.term);
    m_proxyModel.setExtensionFilter(parsed.extension);
    m_proxyModel.setTypeFilter(parsed.type);
    m_proxyModel.setMinSize(parsed.minSize);
    m_proxyModel.setMaxSize(parsed.maxSize);
    m_proxyModel.setMinDate(parsed.minDate);
    m_proxyModel.setMaxDate(parsed.maxDate);
    m_proxyModel.setShowHidden(parsed.showHidden);
    m_proxyModel.setExactMatch(parsed.exact);
    m_cancelRequested = true;
    QPointer<AppController> safeThis(this);
    m_cancelRequested = false;
    auto shouldCancel = [safeThis, generation]() -> bool {
        return !safeThis || safeThis->m_cancelRequested.load() || safeThis->m_operationGeneration.load() != generation;
    };
    ThreadPool::instance().submit([safeThis, parsed, shouldCancel, generation]() {
        QStringList roots;
        if (!safeThis) return;
        
        // Comprehensive search: Home + Mounted volumes
        roots << QDir::homePath();
        for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
            if (!storage.isValid() || !storage.isReady()) continue;
            const QString root = storage.rootPath();
            if (root.startsWith("/proc") || root.startsWith("/sys") || root.startsWith("/dev") || root.startsWith("/run/user")) continue;
            roots << root;
        }
        roots = prunedRoots(roots);

        QSet<QString> seenPaths;
        for (const QString &rootPath : roots) {
            if (shouldCancel()) break;
            const auto publishBatch = [safeThis, generation, &seenPaths](std::vector<FileMeta>&& filtered) {
                if (filtered.empty() || !safeThis) return;
                std::vector<FileMeta> unique;
                unique.reserve(filtered.size());
                QStringList thumbBatch;
                for (auto &entry : filtered) {
                    const QString p = QFileInfo(QString::fromStdString(entry.path)).canonicalFilePath();
                    const QString key = p.isEmpty() ? QString::fromStdString(entry.path) : p;
                    if (seenPaths.contains(key)) continue;
                    seenPaths.insert(key);
                    thumbBatch << key;
                    unique.push_back(std::move(entry));
                }
                if (unique.empty()) return;
                QMetaObject::invokeMethod(safeThis, [safeThis, generation, b = std::move(unique), thumbBatch]() mutable {
                    if (!safeThis || safeThis->m_operationGeneration.load() != generation) return;
                    safeThis->m_fileModel.insertBatch(std::move(b));
                    // Request thumbnails for the batch
                    for (const QString& tPath : thumbBatch) {
                        if (safeThis->m_thumbnailManager) safeThis->m_thumbnailManager->requestThumbnail(tPath);
                    }
                }, Qt::QueuedConnection);
            };

            const bool usedFd = runFdSearch(rootPath, parsed, shouldCancel, publishBatch);
            if (!usedFd) {
                const std::string enginePattern = parsed.term.toStdString();
                FileSystemEngine::searchRecursive(rootPath.toStdString(), enginePattern, [publishBatch, parsed](std::vector<FileMeta>&& batch) {
                    std::vector<FileMeta> filtered;
                    filtered.reserve(batch.size());
                    for (auto &entry : batch) {
                        if (matchesStructuredFilters(entry, parsed)) filtered.push_back(std::move(entry));
                    }
                    publishBatch(std::move(filtered));
                }, shouldCancel);
            }
        }
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, generation]() {
                if (safeThis) {
                    if (safeThis->m_operationGeneration.load() != generation) return;
                    safeThis->m_loading = false;
                    safeThis->m_searchInProgress = false;
                    emit safeThis->loadingChanged();
                    emit safeThis->searchInProgressChanged();
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::setSearchMode(const QString &mode) {
    QString normalized = mode.trimmed().toLower();
    if (normalized != "global") normalized = "local";
    if (m_searchMode == normalized) return;
    m_searchMode = normalized;
    if (m_searchMode == "local" && m_globalSearchActive && !m_currentPath.isEmpty()) {
        loadPathInternal(m_currentPath);
    }
    emit searchModeChanged();
}

void AppController::setSearchScope(const QString &scope)
{
    QString normalized = scope.trimmed().toLower();
    if (normalized != "home" && normalized != "mounted") normalized = "current";
    if (m_searchScope == normalized) return;
    m_searchScope = normalized;
    emit searchScopeChanged();
}

void AppController::applySearchQuery(const QString &query) {
    if (m_searchMode == "global") startGlobalSearch(query);
    else setSearchText(query);
}

void AppController::saveSearchQuery(const QString &query) {
    const QString q = query.trimmed();
    if (q.isEmpty()) return;
    m_recentSearches.removeAll(q);
    m_recentSearches.prepend(q);
    while (m_recentSearches.size() > 10) m_recentSearches.removeLast();
    persistRecentSearches();
    emit recentSearchesChanged();
}

void AppController::cancelSearch()
{
    m_cancelRequested = true;
    ++m_operationGeneration;
    if (m_loading || m_searchInProgress) {
        m_loading = false;
        m_searchInProgress = false;
        emit loadingChanged();
        emit searchInProgressChanged();
    }
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

void AppController::selectRangeByIndexes(int from, int to)
{
    const int rowCount = m_proxyModel.rowCount();
    if (rowCount <= 0) return;
    from = qBound(0, from, rowCount - 1);
    to = qBound(0, to, rowCount - 1);
    if (from > to) std::swap(from, to);

    m_selectedPaths.clear();
    for (int i = from; i <= to; ++i) {
        const QModelIndex proxyIdx = m_proxyModel.index(i, 0);
        const QString path = m_proxyModel.data(proxyIdx, FileListModel::PathRole).toString();
        if (!path.isEmpty()) m_selectedPaths.append(path);
    }
    m_selectedPath = m_selectedPaths.isEmpty() ? "" : m_selectedPaths.last();
    emit selectedPathChanged();
    emit selectedPathsChanged();
}

QString AppController::pathAtIndex(int index) const
{
    const int rowCount = m_proxyModel.rowCount();
    if (index < 0 || index >= rowCount) return "";
    return m_proxyModel.data(m_proxyModel.index(index, 0), FileListModel::PathRole).toString();
}

QString AppController::nameAtIndex(int index) const
{
    const int rowCount = m_proxyModel.rowCount();
    if (index < 0 || index >= rowCount) return "";
    return m_proxyModel.data(m_proxyModel.index(index, 0), FileListModel::NameRole).toString();
}

int AppController::indexOfPath(const QString &path) const
{
    if (path.isEmpty()) return -1;
    const int rowCount = m_proxyModel.rowCount();
    for (int i = 0; i < rowCount; ++i) {
        if (pathAtIndex(i) == path) return i;
    }
    return -1;
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

void AppController::openInCode(const QString &path) { 
    QString target = path;
    if (!m_selectedPath.isEmpty()) target = m_selectedPath;
    else if (target.isEmpty()) target = m_currentPath;

    if (!QProcess::startDetached("code", {target})) emit operationError("VS Code not found"); 
}

void AppController::addToBookmarks(const QString &path, const QString &name) {
    if (!m_placesModel) return;
    m_placesModel->addBookmark(path, name);
    ++m_bookmarksRevision;
    emit bookmarksRevisionChanged();
    emit operationSuccess("Bookmark added");
}
void AppController::removeFromBookmarks(int index) {
    if (!m_placesModel) return;
    m_placesModel->removeBookmark(index);
    ++m_bookmarksRevision;
    emit bookmarksRevisionChanged();
}
void AppController::removeBookmarkByPath(const QString &path) {
    if (!m_placesModel || path.isEmpty()) return;
    m_placesModel->removeBookmarkByPath(path);
    ++m_bookmarksRevision;
    emit bookmarksRevisionChanged();
    emit operationSuccess("Bookmark removed");
}
bool AppController::isBookmarked(const QString &path) const {
    if (!m_placesModel || path.isEmpty()) return false;
    return m_placesModel->isBookmarked(path);
}
void AppController::toggleBookmark(const QString &path, const QString &name) {
    if (!m_placesModel || path.isEmpty()) return;
    if (m_placesModel->isBookmarked(path)) {
        m_placesModel->removeBookmarkByPath(path);
        ++m_bookmarksRevision;
        emit bookmarksRevisionChanged();
        emit operationSuccess("Bookmark removed");
    } else {
        m_placesModel->addBookmark(path, name);
        ++m_bookmarksRevision;
        emit bookmarksRevisionChanged();
        emit operationSuccess("Bookmark added");
    }
}

void AppController::openAsRoot(const QString &path)
{
    if (getuid() == 0) { emit operationError("Already running as root"); return; }
    static bool isElevating = false; if (isElevating) return; isElevating = true;
    QString self = QCoreApplication::applicationFilePath();
    if (!QProcess::startDetached("pkexec", {self, path})) { emit operationError("Failed to launch as root"); isElevating = false; }
    else { emit operationSuccess("Launching elevated instance..."); QTimer::singleShot(5000, []() { isElevating = false; }); }
}

void AppController::openInNewWindow(const QString &path)
{
    QString arg = path.trimmed();
    if (arg.isEmpty()) arg = m_currentPath;
    const QString self = QCoreApplication::applicationFilePath();
    if (!QProcess::startDetached(self, {arg})) emit operationError("Failed to open new window");
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

void AppController::loadRecentSearches()
{
    QSettings settings("Liphis", "Search");
    m_recentSearches = settings.value("recent").toStringList();
}

void AppController::persistRecentSearches()
{
    QSettings settings("Liphis", "Search");
    settings.setValue("recent", m_recentSearches);
}
