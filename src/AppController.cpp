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
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>
#include <unistd.h>
#include <git2.h>
#include <archive.h>
#include <archive_entry.h>

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

QString permissionsToString(const QFile::Permissions perms) {
    auto bit = [perms](QFile::Permission p, const char c) -> QChar {
        return (perms & p) ? QChar(c) : QChar('-');
    };
    QString s;
    s.reserve(9);
    s.append(bit(QFile::ReadOwner, 'r'));
    s.append(bit(QFile::WriteOwner, 'w'));
    s.append(bit(QFile::ExeOwner, 'x'));
    s.append(bit(QFile::ReadGroup, 'r'));
    s.append(bit(QFile::WriteGroup, 'w'));
    s.append(bit(QFile::ExeGroup, 'x'));
    s.append(bit(QFile::ReadOther, 'r'));
    s.append(bit(QFile::WriteOther, 'w'));
    s.append(bit(QFile::ExeOther, 'x'));
    return s;
}

bool commandExists(const QString &command)
{
    QProcess p;
    p.start("sh", {"-lc", "command -v " + command});
    if (!p.waitForFinished(600)) return false;
    return p.exitCode() == 0 && !p.readAllStandardOutput().trimmed().isEmpty();
}
}

AppController::AppController(QObject *parent)
    : QObject(parent)
{
    m_proxyModel.setSourceModel(&m_fileModel);
    loadRecentSearches();
    m_lastUserInitiatedOp.start();

    m_gitStatusTimer = new QTimer(this);
    m_gitStatusTimer->setSingleShot(true);
    m_gitStatusTimer->setInterval(500); // Debounce git status updates
    connect(m_gitStatusTimer, &QTimer::timeout, this, [this]() {
        this->updateGitStatus();
    });

    connect(&m_historyStack, &FileCommandHistory::canUndoChanged, this, &AppController::canUndoChanged);
    connect(&m_historyStack, &FileCommandHistory::canRedoChanged, this, &AppController::canRedoChanged);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(250); // 250ms debounce for search
    connect(m_searchTimer, &QTimer::timeout, this, [this]() {
        if (m_proxyModel.searchQuery() != m_activeSearchTerm) {
            m_proxyModel.setSearchQuery(m_activeSearchTerm);
        }
    });

    // File Watcher
    m_dirChangeTimer = new QTimer(this);
    m_dirChangeTimer->setSingleShot(true);
    m_dirChangeTimer->setInterval(45);
    connect(m_dirChangeTimer, &QTimer::timeout, this, [this]() {
        // Avoid thrashing: if we just performed an operation, our model updates already cover it.
        if (m_lastUserInitiatedOp.isValid() && m_lastUserInitiatedOp.elapsed() < 140) return;
        reloadCurrentDirectoryModel(/*preserveSelection*/ true);
    });

    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
        if (path == m_currentPath) {
            m_dirChangeTimer->start();
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
QObject* AppController::treeModel() { return &m_treeModel; }
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
bool AppController::canUndo() const { return m_historyStack.canUndo(); }
bool AppController::canRedo() const { return m_historyStack.canRedo(); }
QString AppController::undoDescription() const { return m_historyStack.undoDescription(); }

void AppController::undo() { m_historyStack.undo(); refresh(); }
void AppController::redo() { m_historyStack.redo(); refresh(); }

bool AppController::showHiddenFiles() const { return m_proxyModel.showHidden(); }
void AppController::setShowHiddenFiles(bool show) { m_proxyModel.setShowHidden(show); }
bool AppController::hasClipboard() const { return !m_clipboardPaths.isEmpty(); }
QStringList AppController::clipboardPaths() const { return m_clipboardPaths; }
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
    
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, path]() {
        if (!safeThis) return;
        QVariantMap statusMap;
        
        git_repository *repo = nullptr;
        if (git_repository_open_ext(&repo, path.toStdString().c_str(), 0, nullptr) == 0) {
            git_status_options opts = GIT_STATUS_OPTIONS_INIT;
            opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
            opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
            
            git_status_list *statuses = nullptr;
            if (git_status_list_new(&statuses, repo, &opts) == 0) {
                size_t count = git_status_list_entrycount(statuses);
                for (size_t i = 0; i < count; ++i) {
                    const git_status_entry *s = git_status_byindex(statuses, i);
                    if (s->status == GIT_STATUS_CURRENT) continue;
                    
                    QString file = QString::fromUtf8(s->index_to_workdir ? s->index_to_workdir->new_file.path : s->head_to_index->new_file.path);
                    QString state = "  ";
                    
                    if (s->status & GIT_STATUS_WT_NEW) state = "??";
                    else {
                        if (s->status & GIT_STATUS_INDEX_NEW) state[0] = 'A';
                        else if (s->status & GIT_STATUS_INDEX_MODIFIED) state[0] = 'M';
                        else if (s->status & GIT_STATUS_INDEX_DELETED) state[0] = 'D';
                        
                        if (s->status & GIT_STATUS_WT_MODIFIED) state[1] = 'M';
                        else if (s->status & GIT_STATUS_WT_DELETED) state[1] = 'D';
                    }
                    statusMap[file] = state;
                }
                git_status_list_free(statuses);
            }
            git_repository_free(repo);
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
    } else if (asUrl.isValid() && asUrl.scheme() == "trash") {
        resolvedPath = asUrl.toString();
    } else if (resolvedPath.startsWith("file://")) {
        resolvedPath.remove(0, QString("file://").size());
    }

    if (resolvedPath.isEmpty() || resolvedPath == m_currentPath) return;

    if (isTrashPath(resolvedPath)) {
        if (m_historyIndex < m_history.size() - 1) m_history.resize(m_historyIndex + 1);
        m_history.append("trash:///");
        m_historyIndex++;
        emit canGoBackChanged();
        emit canGoForwardChanged();
        loadPathInternal("trash:///");
        return;
    }

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

void AppController::openWith(const QString &path)
{
    if (path.isEmpty()) return;
    emit openWithRequested(path);
}

QString AppController::safePath(const QString &path) const
{
    if (path.isEmpty()) return "";
    QString cleaned = QDir::cleanPath(path.trimmed());
    
    // Convert to absolute path if it's relative
    if (QDir::isRelativePath(cleaned)) {
        cleaned = QDir::cleanPath(m_currentPath + "/" + cleaned);
    }

    // Basic protection against directory traversal via symbols if needed, 
    // but cleanPath already handles "../".
    // We also check that it doesn't try to go outside of allowed roots 
    // if we had a "sandbox" mode, but here we are a file manager.
    return cleaned;
}

void AppController::openWithApp(const QString &path, const QString &appCommand)
{
    const QString targetPath = safePath(path);
    if (targetPath.isEmpty() || appCommand.isEmpty()) return;

    QString cmdLine = appCommand;
    // Desktop files use %f, %F, %u, %U field codes.
    // We should split the command into arguments and replace placeholders.
    // To do this securely without a shell, we need to parse the command string.
    
    QStringList args;
    QString executable;
    
    // Simplified desktop-entry exec parsing (handles quotes)
    static QRegularExpression re("([^\\s\"']+|\"[^\"]*\"|'[^']*')");
    QRegularExpressionMatchIterator it = re.globalMatch(cmdLine);
    
    while (it.hasNext()) {
        QString match = it.next().captured(1);
        if (match.startsWith("\"") && match.endsWith("\"")) match = match.mid(1, match.size() - 2);
        else if (match.startsWith("'") && match.endsWith("'")) match = match.mid(1, match.size() - 2);

        if (executable.isEmpty()) {
            executable = match;
        } else {
            if (match == "%f" || match == "%F") args << targetPath;
            else if (match == "%u" || match == "%U") args << QUrl::fromLocalFile(targetPath).toString();
            else if (match.startsWith("%")) { /* skip other codes */ }
            else args << match;
        }
    }

    // If no placeholder was found, append the path as the last argument
    if (!cmdLine.contains("%f") && !cmdLine.contains("%F") && !cmdLine.contains("%u") && !cmdLine.contains("%U")) {
        args << targetPath;
    }

    if (!executable.isEmpty()) {
        QProcess::startDetached(executable, args);
    }
}

static QVariantMap parseDesktopFile(const QString& desktopFile)
{
    QString fullPath = "/usr/share/applications/" + desktopFile;
    if (!QFile::exists(fullPath)) fullPath = QDir::homePath() + "/.local/share/applications/" + desktopFile;
    if (!QFile::exists(fullPath)) {
        // Search in other common locations
        QStringList paths = {"/usr/local/share/applications/", "/var/lib/flatpak/exports/share/applications/"};
        for (const auto& p : paths) {
            if (QFile::exists(p + desktopFile)) {
                fullPath = p + desktopFile;
                break;
            }
        }
    }

    QSettings desktop(fullPath, QSettings::IniFormat);
    desktop.beginGroup("Desktop Entry");
    QVariantMap app;
    app["name"] = desktop.value("Name").toString();
    if (app["name"].toString().isEmpty()) app["name"] = desktopFile;
    app["icon"] = desktop.value("Icon").toString();
    app["exec"] = desktop.value("Exec").toString();
    app["id"] = desktopFile;
    app["comment"] = desktop.value("Comment").toString();
    return app;
}

QVariantList AppController::getAssociatedApps(const QString &path)
{
    QVariantList apps;
    if (path.isEmpty()) return apps;

    QMimeDatabase mimeDb;
    QMimeType mime = mimeDb.mimeTypeForFile(path);
    QString mimeName = mime.name();

    QProcess gio;
    gio.start("gio", {"mime", mimeName});
    if (gio.waitForFinished()) {
        QString output = gio.readAllStandardOutput();
        QStringList lines = output.split("\n");
        
        QString defaultAppId;
        QStringList registeredApps;
        
        bool inRegistered = false;
        bool inRecommended = false;

        for (const QString &line : lines) {
            QString trimmed = line.trimmed();
            if (line.startsWith("Default application for")) {
                int colonIdx = line.lastIndexOf(':');
                if (colonIdx != -1) defaultAppId = line.mid(colonIdx + 1).trimmed();
            } else if (trimmed == "Registered applications:") {
                inRegistered = true; inRecommended = false;
            } else if (trimmed == "Recommended applications:") {
                inRecommended = true; inRegistered = false;
            } else if ((inRegistered || inRecommended) && trimmed.endsWith(".desktop")) {
                if (!registeredApps.contains(trimmed)) registeredApps.append(trimmed);
            }
        }

        // Put default app first if found
        if (!defaultAppId.isEmpty()) {
            QVariantMap app = parseDesktopFile(defaultAppId);
            app["isDefault"] = true;
            apps.append(app);
            registeredApps.removeAll(defaultAppId);
        }

        for (const QString &appId : registeredApps) {
            QVariantMap app = parseDesktopFile(appId);
            app["isDefault"] = false;
            apps.append(app);
        }
    }

    return apps;
}

QVariantList AppController::getAllApplications()
{
    QVariantList apps;
    QStringList searchPaths = {
        "/usr/share/applications/",
        "/usr/local/share/applications/",
        QDir::homePath() + "/.local/share/applications/",
        "/var/lib/flatpak/exports/share/applications/"
    };

    QStringList seenIds;

    for (const QString &dirPath : searchPaths) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        QStringList files = dir.entryList({"*.desktop"}, QDir::Files);
        for (const QString &file : files) {
            if (seenIds.contains(file)) continue;
            
            QVariantMap app = parseDesktopFile(file);
            if (!app["name"].toString().isEmpty() && !app["exec"].toString().isEmpty()) {
                apps.append(app);
                seenIds.append(file);
            }
        }
    }

    // Sort by name
    std::sort(apps.begin(), apps.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap()["name"].toString().compare(b.toMap()["name"].toString(), Qt::CaseInsensitive) < 0;
    });

    return apps;
}

void AppController::setDefaultApp(const QString &mimeType, const QString &desktopFile)
{
    if (mimeType.isEmpty() || desktopFile.isEmpty()) return;
    QProcess::startDetached("gio", {"mime", mimeType, desktopFile});
}

QString AppController::getMimeType(const QString &path)
{
    if (path.isEmpty()) return "";
    QMimeDatabase db;
    return db.mimeTypeForFile(path).name();
}

void AppController::loadPathInternal(const QString &path)
{
    if (isTrashPath(path)) {
        loadTrashInternal();
        return;
    }

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
    if (!path.isEmpty()) {
        QFileInfo fi(path);
        if (fi.isDir() && fi.isReadable()) {
            m_watcher.addPath(path);
        }
    }
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
                    safeThis->m_gitStatusTimer->start();
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::reloadCurrentDirectoryModel(bool preserveSelection)
{
    if (m_currentPath.isEmpty() || isTrashPath(m_currentPath)) return;
    QFileInfo fi(m_currentPath);
    if (!fi.isDir() || !fi.isReadable()) return;

    const QStringList prevSelected = preserveSelection ? m_selectedPaths : QStringList{};
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, path = m_currentPath, prevSelected, preserveSelection]() {
        if (!safeThis) return;
        std::vector<FileMeta> entries = FileSystemEngine::listDirectorySync(path.toStdString(), 5000);
        QMetaObject::invokeMethod(safeThis, [safeThis, entries = std::move(entries), prevSelected, preserveSelection]() mutable {
            if (!safeThis) return;
            safeThis->m_fileModel.setEntries(std::move(entries));
            emit safeThis->availableExtensionsChanged();
            safeThis->m_gitStatusTimer->start();

            if (!preserveSelection) {
                safeThis->clearSelection();
                return;
            }

            QStringList kept;
            for (const QString &p : prevSelected) {
                if (QFileInfo::exists(p)) kept.append(p);
            }
            if (kept != safeThis->m_selectedPaths) {
                safeThis->m_selectedPaths = kept;
                safeThis->m_selectedPath = kept.isEmpty() ? "" : kept.last();
                safeThis->m_selectionRevision++;
                emit safeThis->selectedPathChanged();
                emit safeThis->selectedPathsChanged();
            }
        }, Qt::QueuedConnection);
    });
}

void AppController::removeFromSelection(const QStringList &paths)
{
    if (paths.isEmpty() || m_selectedPaths.isEmpty()) return;
    bool changed = false;
    for (const QString &p : paths) {
        const int before = m_selectedPaths.size();
        m_selectedPaths.removeAll(p);
        if (m_selectedPaths.size() != before) changed = true;
    }
    if (!changed) return;
    const QString next = m_selectedPaths.isEmpty() ? QString() : m_selectedPaths.last();
    if (next != m_selectedPath) {
        m_selectedPath = next;
        emit selectedPathChanged();
    }
    m_selectionRevision++;
    emit selectedPathsChanged();
}

void AppController::addEntriesForPaths(const QStringList &paths, bool selectAdded)
{
    if (paths.isEmpty()) return;
    std::vector<FileMeta> newEntries;
    newEntries.reserve(static_cast<size_t>(paths.size()));

    QStringList addedPaths;
    for (const QString &path : paths) {
        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        if (!fi.exists()) continue;
        if (fi.absolutePath() != m_currentPath) continue;
        if (m_fileModel.metadataForPath(path).isEmpty() == false) continue;
        newEntries.push_back(FileSystemEngine::getFileMeta(path.toStdString()));
        addedPaths << path;
    }

    if (!newEntries.empty()) {
        m_fileModel.insertBatch(std::move(newEntries));
        emit availableExtensionsChanged();
    }

    if (m_thumbnailManager) {
        for (const QString &p : addedPaths) {
            m_thumbnailManager->requestThumbnail(p);
        }
    }

    if (selectAdded && !addedPaths.isEmpty()) {
        m_selectedPaths = addedPaths;
        m_selectedPath = addedPaths.last();
        m_selectionRevision++;
        emit selectedPathChanged();
        emit selectedPathsChanged();
    }
}

bool AppController::isTrashPath(const QString &path) const
{
    const QString p = path.trimmed();
    return p == "trash:///" || p.startsWith("trash:");
}

static QString userTrashFilesDir()
{
    return QDir::homePath() + "/.local/share/Trash/files";
}

static QString userTrashInfoDir()
{
    return QDir::homePath() + "/.local/share/Trash/info";
}

void AppController::loadTrashInternal()
{
    const std::uint64_t generation = ++m_operationGeneration;
    m_loading = true;
    emit loadingChanged();

    m_cancelRequested = true;
    if (!m_currentPath.isEmpty()) m_watcher.removePath(m_currentPath);
    m_currentPath = "trash:///";
    emit currentPathChanged();
    emit titleChanged();
    clearSelection();
    m_fileModel.clear();

    const QString trashDir = userTrashFilesDir();
    QPointer<AppController> safeThis(this);
    m_cancelRequested = false;

    ThreadPool::instance().submit([safeThis, trashDir, generation]() {
        if (!safeThis) return;
        QDir dir(trashDir);
        QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden, QDir::Name);
        std::vector<FileMeta> entries;
        entries.reserve(static_cast<size_t>(list.size()));

        for (const QFileInfo &fi : list) {
            FileMeta m;
            m.name = fi.fileName().toStdString();
            m.path = fi.absoluteFilePath().toStdString();
            m.isDir = fi.isDir();
            m.size = static_cast<std::uint64_t>(fi.size());
            m.mtime = static_cast<std::uint64_t>(fi.lastModified().toSecsSinceEpoch());
            m.ctime = static_cast<std::uint64_t>(fi.metadataChangeTime().toSecsSinceEpoch());
            m.atime = static_cast<std::uint64_t>(fi.lastRead().toSecsSinceEpoch());
            entries.emplace_back(std::move(m));
        }

        QMetaObject::invokeMethod(safeThis, [safeThis, generation, entries = std::move(entries)]() mutable {
            if (!safeThis || safeThis->m_operationGeneration.load() != generation) return;
            safeThis->m_fileModel.setEntries(std::move(entries));
            safeThis->m_loading = false;
            emit safeThis->loadingChanged();
            emit safeThis->availableExtensionsChanged();
        }, Qt::QueuedConnection);
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
    
    // We update proxy filters immediately as they are fast, 
    // but we debounce the actual text search which invalidates everything.
    m_proxyModel.setExtensionFilter(parsed.extension);
    m_proxyModel.setTypeFilter(parsed.type);
    m_proxyModel.setMinSize(parsed.minSize);
    m_proxyModel.setMaxSize(parsed.maxSize);
    m_proxyModel.setMinDate(parsed.minDate);
    m_proxyModel.setMaxDate(parsed.maxDate);
    m_proxyModel.setShowHidden(parsed.showHidden);
    m_proxyModel.setExactMatch(parsed.exact);

    if (m_activeSearchTerm != parsed.term) {
        m_activeSearchTerm = parsed.term;
        emit activeSearchTermChanged();
        if (m_activeSearchTerm.isEmpty()) {
            m_searchTimer->stop();
            m_proxyModel.setSearchQuery("");
        } else {
            m_searchTimer->start();
        }
    }
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
    const bool searchInFiles = m_searchContent && !parsed.term.isEmpty();

    const QString scope = m_searchScope;
    const QString currentPath = m_currentPath;
    ThreadPool::instance().submit([safeThis, parsed, shouldCancel, generation, searchInFiles, scope, currentPath]() {
        QStringList roots;
        if (!safeThis) return;

        if (scope == "current") {
            if (!currentPath.isEmpty()) roots << currentPath;
        } else if (scope == "home") {
            roots << QDir::homePath();
        } else { // mounted
            for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
                if (!storage.isValid() || !storage.isReady()) continue;
                const QString root = storage.rootPath();
                if (root.startsWith("/proc") || root.startsWith("/sys") || root.startsWith("/dev") || root.startsWith("/run/user")) continue;
                roots << root;
            }
        }
        if (roots.isEmpty()) roots << QDir::homePath();
        roots = prunedRoots(roots);

        const bool hasFd = commandExists("fd");
        const bool hasRg = commandExists("rg");
        QSet<QString> seenPaths;
        int thumbnailBudget = 60;
        static constexpr int kMaxGlobalResults = 3500;
        int emittedCount = 0;
        bool hitResultLimit = false;
        for (const QString &rootPath : roots) {
            if (shouldCancel() || hitResultLimit) break;
            
            const auto publishBatch = [safeThis, generation, &seenPaths, &thumbnailBudget, &emittedCount, &hitResultLimit](std::vector<FileMeta>&& filtered) {
                if (filtered.empty() || !safeThis) return;
                std::vector<FileMeta> unique;
                unique.reserve(filtered.size());
                QStringList thumbBatch;
                for (auto &entry : filtered) {
                    if (emittedCount >= kMaxGlobalResults) {
                        hitResultLimit = true;
                        break;
                    }
                    const QString key = QString::fromStdString(entry.path);
                    if (seenPaths.contains(key)) continue;
                    seenPaths.insert(key);
                    if (thumbnailBudget > 0) {
                        thumbBatch << key;
                        --thumbnailBudget;
                    }
                    unique.push_back(std::move(entry));
                    emittedCount++;
                }
                if (emittedCount >= kMaxGlobalResults) {
                    hitResultLimit = true;
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

            if (searchInFiles) {
                if (hasRg) {
                    QProcess rg;
                    QStringList args;
                    args << "--files-with-matches" << "--smart-case"
                         << "-g" << "!.git/*" << "-g" << "!node_modules/*" << "-g" << "!.cache/*";
                    if (parsed.showHidden) args << "--hidden";
                    args << parsed.term << rootPath;
                    rg.start("rg", args);
                    if (rg.waitForStarted()) {
                        std::vector<FileMeta> currentBatch;
                        while (!rg.atEnd() || rg.state() == QProcess::Running) {
                            if (shouldCancel()) {
                                rg.terminate();
                                rg.waitForFinished(800);
                                return;
                            }
                            while (rg.canReadLine()) {
                                const QString path = QString::fromUtf8(rg.readLine()).trimmed();
                                if (path.isEmpty()) continue;
                                FileMeta m = FileSystemEngine::getFileMeta(path.toStdString());
                                if (matchesStructuredFilters(m, parsed)) currentBatch.push_back(std::move(m));
                            }
                            if (currentBatch.size() >= 40) {
                                publishBatch(std::move(currentBatch));
                                currentBatch.clear();
                                if (hitResultLimit) {
                                    rg.terminate();
                                    rg.waitForFinished(800);
                                    break;
                                }
                            }
                            rg.waitForReadyRead(30);
                        }
                        if (!currentBatch.empty()) publishBatch(std::move(currentBatch));
                    }
                } else {
                    // Fallback to grep if ripgrep is unavailable
                    std::vector<FileMeta> currentBatch;
                    QProcess grep;
                    QStringList args;
                    args << "-r" << "-l" << "-i" << "--exclude-dir=.git" << "--exclude-dir=node_modules";
                    if (!parsed.showHidden) args << "--exclude-dir=.*";
                    args << parsed.term << rootPath;
                    grep.start("grep", args);
                    if (!grep.waitForStarted()) continue;
                    while (!grep.atEnd() || grep.state() == QProcess::Running) {
                        if (shouldCancel()) { 
                            grep.terminate(); 
                            grep.waitForFinished(800); 
                            return; 
                        }
                        while (grep.canReadLine()) {
                            QString path = QString::fromUtf8(grep.readLine()).trimmed();
                            if (!path.isEmpty()) {
                                FileMeta m = FileSystemEngine::getFileMeta(path.toStdString());
                                if (matchesStructuredFilters(m, parsed)) currentBatch.push_back(std::move(m));
                            }
                        }
                        if (currentBatch.size() >= 40) {
                            publishBatch(std::move(currentBatch));
                            currentBatch.clear();
                            if (hitResultLimit) {
                                grep.terminate();
                                grep.waitForFinished(800);
                                break;
                            }
                        }
                        grep.waitForReadyRead(30);
                    }
                    if (!currentBatch.empty()) publishBatch(std::move(currentBatch));
                }
            } else {
                if (hasFd) {
                    QProcess fd;
                    QStringList args;
                    args << "--absolute-path" << "--color=never" << "--hidden" << "--no-ignore-vcs"
                         << "--exclude" << ".git" << "--exclude" << "node_modules" << "--exclude" << ".cache";
                    if (!parsed.showHidden) args << "--exclude" << ".*";
                    if (parsed.type == "folder") args << "--type=d";
                    else if (parsed.type == "file") args << "--type=f";
                    args << (parsed.term.isEmpty() ? "." : parsed.term) << rootPath;

                    fd.start("fd", args);
                    if (fd.waitForStarted()) {
                        std::vector<FileMeta> currentBatch;
                        while (!fd.atEnd() || fd.state() == QProcess::Running) {
                            if (shouldCancel()) {
                                fd.terminate();
                                fd.waitForFinished(800);
                                return;
                            }
                            while (fd.canReadLine()) {
                                const QString p = QString::fromUtf8(fd.readLine()).trimmed();
                                if (p.isEmpty()) continue;
                                FileMeta meta = FileSystemEngine::getFileMeta(p.toStdString());
                                if (matchesStructuredFilters(meta, parsed)) currentBatch.push_back(std::move(meta));
                            }
                            if (currentBatch.size() >= 60) {
                                publishBatch(std::move(currentBatch));
                                currentBatch.clear();
                                if (hitResultLimit) {
                                    fd.terminate();
                                    fd.waitForFinished(800);
                                    break;
                                }
                            }
                            fd.waitForReadyRead(30);
                        }
                        if (!currentBatch.empty()) publishBatch(std::move(currentBatch));
                    }
                } else {
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

void AppController::setSearchContent(bool enabled)
{
    if (m_searchContent == enabled) return;
    m_searchContent = enabled;
    emit searchContentChanged();
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
    m_historyStack.push(std::make_unique<CreateFolderCommand>(fullPath.toStdString()));
    m_lastUserInitiatedOp.restart();
    addEntriesForPaths({fullPath}, /*selectAdded*/ true);
}

void AppController::createFile(const QString &name)
{
    if (name.isEmpty() || m_currentPath.isEmpty()) return;
    QString fullPath = m_currentPath + "/" + name;
    m_historyStack.push(std::make_unique<CreateFileCommand>(fullPath.toStdString()));
    m_lastUserInitiatedOp.restart();
    addEntriesForPaths({fullPath}, /*selectAdded*/ true);
}

void AppController::renameItem(const QString &oldPath, const QString &newName)
{
    if (oldPath.isEmpty() || newName.isEmpty()) return;
    QFileInfo fi(oldPath);
    QString newPath = fi.absolutePath() + "/" + newName;
    m_historyStack.push(std::make_unique<RenameCommand>(oldPath.toStdString(), newPath.toStdString()));
    m_lastUserInitiatedOp.restart();
    const bool oldInCurrent = (fi.absolutePath() == m_currentPath);
    const bool newInCurrent = (QFileInfo(newPath).absolutePath() == m_currentPath);
    if (oldInCurrent) {
        m_fileModel.removeItems({oldPath});
        removeFromSelection({oldPath});
    }
    if (newInCurrent) {
        addEntriesForPaths({newPath}, /*selectAdded*/ true);
    }
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
    const QString target = safePath(path);
    if (target.isEmpty()) return;
    auto res = FileSystemEngine::deletePath(target.toStdString());
    if (res.success) { 
        m_fileModel.removeItems({path});
        removeFromSelection({path});
        emit operationSuccess("Deleted"); 
    }
    else emit operationError(QString::fromStdString(res.message));
}

void AppController::deleteItems(const QStringList &paths)
{
    if (paths.isEmpty()) return;
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, paths]() {
        bool allSuccess = true;
        QString lastError;
        for (const QString &path : paths) {
            const QString target = safeThis ? safeThis->safePath(path) : path;
            auto res = FileSystemEngine::deletePath(target.toStdString());
            if (!res.success) { allSuccess = false; lastError = QString::fromStdString(res.message); }
        }
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, allSuccess, lastError, paths]() {
                if (safeThis) {
                    safeThis->m_fileModel.removeItems(paths);
                    safeThis->removeFromSelection(paths);
                    if (allSuccess) emit safeThis->operationSuccess("Deleted multiple items");
                    else emit safeThis->operationError("Some items could not be deleted: " + lastError);
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::copyItem(const QString &path)
{
    m_clipboardPaths.clear();
    if (path.isEmpty()) {
        if (!m_selectedPaths.isEmpty()) m_clipboardPaths = m_selectedPaths;
    } else {
        m_clipboardPaths.append(path);
    }
    m_isCutOp = false;
    emit clipboardChanged();
    if (!m_clipboardPaths.isEmpty()) emit operationSuccess(QString("Copied %1 items").arg(m_clipboardPaths.size()));
}

void AppController::cutItem(const QString &path)
{
    m_clipboardPaths.clear();
    if (path.isEmpty()) {
        if (!m_selectedPaths.isEmpty()) m_clipboardPaths = m_selectedPaths;
    } else {
        m_clipboardPaths.append(path);
    }
    m_isCutOp = true;
    emit clipboardChanged();
    if (!m_clipboardPaths.isEmpty()) emit operationSuccess(QString("Cut %1 items").arg(m_clipboardPaths.size()));
}

void AppController::clearClipboard()
{
    m_clipboardPaths.clear();
    emit clipboardChanged();
}

void AppController::pasteItem()
{
    if (m_clipboardPaths.isEmpty() || m_currentPath.isEmpty()) return;
    QStringList srcs = m_clipboardPaths;
    QString destDir = m_currentPath;
    bool isCut = m_isCutOp;

    if (isCut) {
        std::vector<std::pair<std::string, std::string>> moves;
        for (const auto& s : srcs) {
            moves.push_back({s.toStdString(), (fs::path(destDir.toStdString()) / fs::path(s.toStdString()).filename()).string()});
        }
        m_historyStack.push(std::make_unique<MoveCommand>(moves));
        m_clipboardPaths.clear();
        emit clipboardChanged();
        m_lastUserInitiatedOp.restart();
        QStringList removedFromCurrent;
        QStringList addedToCurrent;
        for (const QString &src : srcs) {
            QFileInfo srcInfo(src);
            const QString dest = QDir(destDir).filePath(srcInfo.fileName());
            if (srcInfo.absolutePath() == m_currentPath) removedFromCurrent << srcInfo.absoluteFilePath();
            if (QFileInfo(dest).absolutePath() == m_currentPath) addedToCurrent << dest;
        }
        if (!removedFromCurrent.isEmpty()) {
            m_fileModel.removeItems(removedFromCurrent);
            removeFromSelection(removedFromCurrent);
        }
        addEntriesForPaths(addedToCurrent, /*selectAdded*/ !addedToCurrent.isEmpty());
        return;
    }

    emit operationProgress(0.0);
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, srcs, destDir, isCut]() {
        bool allSuccess = true;
        QString lastError;
        int count = 0;

        for (const QString &src : srcs) {
            fs::path s(src.toStdString());
            fs::path d = fs::path(destDir.toStdString()) / s.filename();

            FileSystemEngine::OperationResult res;
            if (isCut) res = FileSystemEngine::movePath(src.toStdString(), d.string());
            else res = FileSystemEngine::copyPath(src.toStdString(), d.string());

            if (!res.success) {
                allSuccess = false;
                lastError = QString::fromStdString(res.message);
            }
            count++;
            if (safeThis) QMetaObject::invokeMethod(safeThis, [safeThis, count, total = srcs.size()]() {
                emit safeThis->operationProgress(static_cast<float>(count) / total);
            }, Qt::QueuedConnection);
        }

        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, srcs, destDir, isCut, allSuccess, lastError]() {
                if (safeThis) {
                    emit safeThis->operationProgress(1.0);
                    if (allSuccess) {
                        safeThis->m_lastUserInitiatedOp.restart();
                        QStringList added;
                        for (const QString &src : srcs) {
                            QFileInfo srcInfo(src);
                            const QString dest = QDir(destDir).filePath(srcInfo.fileName());
                            added << dest;
                        }
                        safeThis->addEntriesForPaths(added, /*selectAdded*/ !added.isEmpty());
                        if (isCut) { safeThis->m_clipboardPaths.clear(); emit safeThis->clipboardChanged(); }
                        emit safeThis->operationSuccess("Paste successful");
                    } else emit safeThis->operationError("Paste failed: " + lastError);
                }
            }, Qt::QueuedConnection);
        }
    });
}
void AppController::requestThumbnail(const QString &path) { if (m_thumbnailManager) m_thumbnailManager->requestThumbnail(path); }

void AppController::selectPath(const QString &path)
{
    if (m_selectedPath == path && m_selectedPaths.size() == 1 && m_selectedPaths.contains(path)) return;
    m_selectedPath = path;
    m_selectedPaths.clear();
    if (!path.isEmpty()) m_selectedPaths.append(path);
    m_selectionRevision++;
    emit selectedPathChanged(); emit selectedPathsChanged();
}

void AppController::toggleSelection(const QString &path)
{
    if (m_selectedPaths.contains(path)) m_selectedPaths.removeAll(path);
    else m_selectedPaths.append(path);
    if (m_selectedPaths.isEmpty()) m_selectedPath = "";
    else m_selectedPath = m_selectedPaths.last();
    m_selectionRevision++;
    emit selectedPathChanged(); emit selectedPathsChanged();
}

void AppController::clearSelection()
{
    m_selectedPath = ""; 
    m_selectedPaths.clear();
    m_selectionRevision++;
    emit selectedPathChanged(); 
    emit selectedPathsChanged();
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
    m_selectionRevision++;
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
    m_selectionRevision++;
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
QVariantMap AppController::metadataForPath(const QString &path) const
{
    const QString target = safePath(path);
    if (target.isEmpty()) return {};

    QFileInfo fi(target);
    if (!fi.exists()) return {};

    QVariantMap m = m_fileModel.metadataForPath(target);
    if (m.isEmpty()) {
        m["name"] = fi.fileName().isEmpty() ? "Root" : fi.fileName();
        m["path"] = fi.absoluteFilePath();
        m["isDir"] = fi.isDir();
        m["size"] = static_cast<qlonglong>(fi.size());
        m["iconName"] = fi.isDir() ? "folder" : "text-x-generic";
    }

    m["name"] = fi.fileName().isEmpty() ? "Root" : fi.fileName();
    m["path"] = fi.absoluteFilePath();
    m["isDir"] = fi.isDir();
    m["size"] = static_cast<qlonglong>(fi.size());
    m["mtime"] = static_cast<qlonglong>(fi.lastModified().toSecsSinceEpoch());
    m["ctime"] = static_cast<qlonglong>(fi.metadataChangeTime().toSecsSinceEpoch());
    m["atime"] = static_cast<qlonglong>(fi.lastRead().toSecsSinceEpoch());
    m["birthTime"] = static_cast<qlonglong>(fi.birthTime().toSecsSinceEpoch());
    m["permissions"] = permissionsToString(fi.permissions());
    m["owner"] = fi.owner();
    m["group"] = fi.group();
    m["suffix"] = fi.suffix().toLower();
    m["isReadable"] = fi.isReadable();
    m["isWritable"] = fi.isWritable();
    m["isExecutable"] = fi.isExecutable();
    m["parentPath"] = fi.absolutePath();

    if (fi.isDir()) {
        m["mimeType"] = "inode/directory";
        m["iconName"] = "folder";

        int files = 0;
        int folders = 0;
        qlonglong totalSize = 0;
        const QDir dir(target);
        const QFileInfoList list = dir.entryInfoList(
            QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden,
            QDir::Name
        );
        for (const QFileInfo &entry : list) {
            if (entry.isDir()) folders++;
            else {
                files++;
                totalSize += static_cast<qlonglong>(entry.size());
            }
        }

        m["fileCount"] = files;
        m["folderCount"] = folders;
        m["contentsSize"] = totalSize;
        if (m_folderSizeCache.contains(target)) {
            m["size"] = m_folderSizeCache.value(target);
            m["sizeComputed"] = true;
        } else {
            m["size"] = totalSize;
            m["sizeComputed"] = false;
        }
    } else {
        QMimeDatabase db;
        const QMimeType mime = db.mimeTypeForFile(fi.absoluteFilePath(), QMimeDatabase::MatchContent);
        const QString mimeName = mime.name().isEmpty() ? QString("application/octet-stream") : mime.name();
        m["mimeType"] = mimeName;
        if (m.value("iconName").toString().isEmpty()) {
            m["iconName"] = mime.iconName();
        }
    }

    return m;
}

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
        for (const QString &path : paths) FileSystemEngine::moveToTrash(path.toStdString());
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, paths]() {
                if (safeThis) {
                    safeThis->m_fileModel.removeItems(paths);
                    safeThis->removeFromSelection(paths);
                    safeThis->m_lastUserInitiatedOp.restart();
                    emit safeThis->operationSuccess("Moved to Trash");
                }
            }, Qt::QueuedConnection);
        }
    });
}

static QString readTrashInfoOriginalPath(const QString &trashedAbsolutePath)
{
    const QFileInfo fi(trashedAbsolutePath);
    const QString infoPath = QDir(userTrashInfoDir()).filePath(fi.fileName() + ".trashinfo");
    QFile f(infoPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (!line.startsWith("Path=")) continue;
        const QString encoded = line.mid(QString("Path=").size()).trimmed();
        // Spec stores percent-escaped path, not a file:// URL
        return QUrl::fromPercentEncoding(encoded.toUtf8());
    }
    return {};
}

void AppController::restoreFromTrash(const QStringList &paths)
{
    if (paths.isEmpty()) return;

    // If we are in the Trash view, restore from the user's trash directory.
    if (isTrashPath(m_currentPath)) {
        QPointer<AppController> safeThis(this);
        ThreadPool::instance().submit([safeThis, paths]() {
            bool allSuccess = true;
            QString lastError;

            for (const QString &trashedPath : paths) {
                const QString original = readTrashInfoOriginalPath(trashedPath);
                if (original.isEmpty()) {
                    allSuccess = false;
                    lastError = "Missing .trashinfo for " + trashedPath;
                    continue;
                }

                QString dest = original;
                if (QFileInfo::exists(dest)) {
                    QFileInfo dfi(dest);
                    const QString base = dfi.completeBaseName();
                    const QString ext = dfi.completeSuffix();
                    const QString suffix = ext.isEmpty() ? "" : ("." + ext);
                    dest = dfi.absolutePath() + "/" + base + " (restored)" + suffix;
                }

                const auto res = FileSystemEngine::movePath(trashedPath.toStdString(), dest.toStdString());
                if (!res.success) {
                    allSuccess = false;
                    lastError = QString::fromStdString(res.message);
                }

                // Clean up the .trashinfo if present
                const QFileInfo fi(trashedPath);
                const QString infoPath = QDir(userTrashInfoDir()).filePath(fi.fileName() + ".trashinfo");
                QFile::remove(infoPath);
            }

            if (safeThis) {
                QMetaObject::invokeMethod(safeThis, [safeThis, allSuccess, lastError, paths]() {
                    if (!safeThis) return;
                    safeThis->m_fileModel.removeItems(paths);
                    safeThis->removeFromSelection(paths);
                    safeThis->m_lastUserInitiatedOp.restart();
                    if (allSuccess) emit safeThis->operationSuccess("Items restored");
                    else emit safeThis->operationError("Some items could not be restored: " + lastError);
                }, Qt::QueuedConnection);
            }
        });
        return;
    }

    // Fallback: use gio (useful if we got trash:// URIs from elsewhere)
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, paths]() {
        for (const QString &path : paths) {
            QProcess::execute("gio", {"trash", "--restore", path});
        }
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, paths]() {
                if (!safeThis) return;
                safeThis->m_fileModel.removeItems(paths);
                safeThis->removeFromSelection(paths);
                safeThis->m_lastUserInitiatedOp.restart();
                emit safeThis->operationSuccess("Items restored");
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::emptyTrash()
{
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis]() {
        if (safeThis && safeThis->isTrashPath(safeThis->m_currentPath)) {
            QDir files(userTrashFilesDir());
            QDir info(userTrashInfoDir());
            for (const QFileInfo &fi : files.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden)) {
                FileSystemEngine::deletePath(fi.absoluteFilePath().toStdString());
            }
            for (const QFileInfo &fi : info.entryInfoList({"*.trashinfo"}, QDir::Files | QDir::Hidden)) {
                FileSystemEngine::deletePath(fi.absoluteFilePath().toStdString());
            }
        } else {
            // gio trash --empty is the standard way to empty trash on Linux
            QProcess::execute("gio", {"trash", "--empty"});
        }
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis]() {
                if (safeThis) {
                    safeThis->m_lastUserInitiatedOp.restart();
                    if (safeThis->isTrashPath(safeThis->m_currentPath)) safeThis->loadTrashInternal();
                    else safeThis->refresh();
                    emit safeThis->operationSuccess("Trash emptied");
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::analyseFolder(const QString &path) { emit analyseRequested(path); }

QString AppController::runGitCommand(const QString &cmd, const QString &path)
{
    QString target = safePath(path);
    if (target.isEmpty()) target = m_currentPath;
    
    // Only allow specific safe git commands
    QStringList allowed = {"status", "diff", "branch", "remote", "log", "show"};
    QString baseCmd = cmd.split(" ").first();
    if (!allowed.contains(baseCmd)) return "Command not allowed";

    QProcess git;
    git.setWorkingDirectory(target);
    QStringList args = cmd.split(" ", Qt::SkipEmptyParts);
    
    git.start("git", args);
    if (git.waitForFinished(5000)) {
        return QString::fromUtf8(git.readAllStandardOutput() + git.readAllStandardError());
    }
    return "Command timed out";
}

QVariantMap AppController::getFolderMetadata(const QString &path)
{
    QString target = safePath(path);
    if (target.isEmpty()) target = m_currentPath;

    QFileInfo fi(target);
    if (!fi.exists() || !fi.isDir()) return {};

    QVariantMap m = metadataForPath(target);
    return m;
}

void AppController::requestFolderSize(const QString &path)
{
    const QString target = safePath(path);
    if (target.isEmpty()) return;

    QFileInfo fi(target);
    if (!fi.exists() || !fi.isDir()) return;

    if (m_folderSizeCache.contains(target)) {
        emit folderSizeResolved(target, m_folderSizeCache.value(target));
        return;
    }
    if (m_pendingFolderSizeRequests.contains(target)) return;
    m_pendingFolderSizeRequests.insert(target);

    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, target]() {
        if (!safeThis) return;

        qlonglong totalSize = 0;
        std::error_code ec;
        for (auto it = fs::recursive_directory_iterator(
                 target.toStdString(), fs::directory_options::skip_permission_denied, ec);
             it != fs::recursive_directory_iterator();
             it.increment(ec))
        {
            if (!safeThis) return;
            if (ec) {
                ec.clear();
                continue;
            }
            std::error_code stEc;
            if (!fs::is_regular_file(it->symlink_status(stEc)) || stEc) continue;
            std::error_code szEc;
            const auto sz = fs::file_size(it->path(), szEc);
            if (szEc) continue;
            totalSize += static_cast<qlonglong>(sz);
        }

        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, target, totalSize]() {
                if (!safeThis) return;
                safeThis->m_pendingFolderSizeRequests.remove(target);
                safeThis->m_folderSizeCache.insert(target, totalSize);
                emit safeThis->folderSizeResolved(target, totalSize);
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::compressItems(const QStringList &paths)
{
    if (paths.isEmpty()) return;
    QString parentDir = QFileInfo(paths.first()).absolutePath();
    QString archiveName = parentDir + "/archive.tar.gz";

    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, paths, archiveName, parentDir]() {
        struct archive *a;
        struct archive_entry *entry;
        struct stat st;
        char buff[8192];
        int len;
        FILE *fd;

        a = archive_write_new();
        archive_write_add_filter_gzip(a);
        archive_write_set_format_pax_restricted(a);
        archive_write_open_filename(a, archiveName.toStdString().c_str());

        bool success = true;
        for (const QString& path : paths) {
            std::string stdPath = path.toStdString();
            std::string relPath = QFileInfo(path).fileName().toStdString();
            
            if (stat(stdPath.c_str(), &st) != 0) { success = false; continue; }

            entry = archive_entry_new();
            archive_entry_set_pathname(entry, relPath.c_str());
            archive_entry_set_size(entry, st.st_size);
            archive_entry_set_filetype(entry, st.st_mode);
            archive_entry_set_perm(entry, 0644);
            archive_write_header(a, entry);

            if (S_ISREG(st.st_mode)) {
                fd = fopen(stdPath.c_str(), "rb");
                if (fd) {
                    while ((len = fread(buff, 1, sizeof(buff), fd)) > 0)
                        archive_write_data(a, buff, len);
                    fclose(fd);
                }
            }
            archive_entry_free(entry);
        }
        archive_write_close(a);
        archive_write_free(a);

        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, success]() {
                if (success) { safeThis->refresh(); emit safeThis->operationSuccess("Compressed successfully"); }
                else emit safeThis->operationError("Failed to compress some items");
            }, Qt::QueuedConnection);
        }
    });
}

static int copy_data(struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;
    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) return ARCHIVE_OK;
        if (r < ARCHIVE_OK) return r;
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) return r;
    }
}

void AppController::extractItem(const QString &path)
{
    if (path.isEmpty()) return;
    QFileInfo fi(path); QString dir = fi.absolutePath();

    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, path, dir]() {
        struct archive *a;
        struct archive *ext;
        struct archive_entry *entry;
        int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;
        int r;

        a = archive_read_new();
        archive_read_support_format_all(a);
        archive_read_support_filter_all(a);
        ext = archive_write_disk_new();
        archive_write_disk_set_options(ext, flags);
        archive_write_disk_set_standard_lookup(ext);

        bool success = true;
        if ((r = archive_read_open_filename(a, path.toStdString().c_str(), 10240))) {
            success = false;
        } else {
            // Change dir before extraction
            chdir(dir.toStdString().c_str());
            for (;;) {
                r = archive_read_next_header(a, &entry);
                if (r == ARCHIVE_EOF) break;
                if (r < ARCHIVE_OK) if (r < ARCHIVE_WARN) { success = false; break; }
                r = archive_write_header(ext, entry);
                if (r < ARCHIVE_OK) {
                    if (r < ARCHIVE_WARN) { success = false; break; }
                } else if (archive_entry_size(entry) > 0) {
                    r = copy_data(a, ext);
                    if (r < ARCHIVE_OK) if (r < ARCHIVE_WARN) { success = false; break; }
                }
                r = archive_write_finish_entry(ext);
                if (r < ARCHIVE_OK) if (r < ARCHIVE_WARN) { success = false; break; }
            }
        }
        archive_read_close(a);
        archive_read_free(a);
        archive_write_close(ext);
        archive_write_free(ext);

        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, success]() {
                if (success) { safeThis->refresh(); emit safeThis->operationSuccess("Extracted successfully"); }
                else emit safeThis->operationError("Extraction failed");
            }, Qt::QueuedConnection);
        }
    });
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

void AppController::connectRemote(const QString &url) {
    if (url.isEmpty()) return;
    
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, url]() {
        QProcess proc;
        proc.start("gio", QStringList() << "mount" << url);
        proc.waitForFinished(-1);
        bool success = (proc.exitCode() == 0);
        
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, success, url]() {
                if (success) {
                    emit safeThis->operationSuccess("Connected to " + url);
                    
                    // Attempt to find the mount path
                    // Usually /run/user/UID/gvfs/NAME
                    // We can check mounted volumes
                    QTimer::singleShot(1000, safeThis, [safeThis, url]() {
                        for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
                            if (storage.isValid() && storage.isReady()) {
                                if (storage.rootPath().contains("gvfs")) {
                                    // Try to match URL to mount path
                                    // This is a bit heuristical but often works
                                    safeThis->openPath(storage.rootPath());
                                    break;
                                }
                            }
                        }
                    });
                } else {
                    emit safeThis->operationError("Failed to connect to " + url);
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AppController::mountRemote(const QString &url) { connectRemote(url); }

void AppController::dropItems(const QStringList &paths, const QString &targetDir, bool isCopy)
{
    if (paths.isEmpty() || targetDir.isEmpty()) return;
    
    QPointer<AppController> safeThis(this);
    ThreadPool::instance().submit([safeThis, paths, targetDir, isCopy]() {
        QStringList added;
        QStringList removed;
        for (const QString &src : paths) {
            QString cleanSrc = src;
            if (cleanSrc.startsWith("file://")) cleanSrc = QUrl(src).toLocalFile();
            
            QFileInfo fi(cleanSrc);
            QString dest = targetDir + "/" + fi.fileName();
            
            if (isCopy) {
                FileSystemEngine::copyPath(cleanSrc.toStdString(), dest.toStdString(), nullptr);
            } else {
                FileSystemEngine::movePath(cleanSrc.toStdString(), dest.toStdString());
                removed << fi.absoluteFilePath();
            }
            added << dest;
        }
        
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, added, removed]() {
                if (!safeThis) return;
                safeThis->m_lastUserInitiatedOp.restart();
                if (!removed.isEmpty()) {
                    safeThis->m_fileModel.removeItems(removed);
                    safeThis->removeFromSelection(removed);
                }
                safeThis->addEntriesForPaths(added, /*selectAdded*/ false);
            }, Qt::QueuedConnection);
        }
    });
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
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, res, newPath]() {
                if (!safeThis) return;
                if (res.success) {
                    safeThis->m_lastUserInitiatedOp.restart();
                    safeThis->addEntriesForPaths({newPath}, /*selectAdded*/ true);
                    emit safeThis->operationSuccess("Duplicated");
                } else {
                    emit safeThis->operationError(QString::fromStdString(res.message));
                }
            }, Qt::QueuedConnection);
        }
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
    const QString targetPath = safePath(path);
    if (targetPath.isEmpty()) return;

    QString desktop = QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP").toLower();
    if (desktop.contains("gnome") || desktop.contains("unity")) { 
        QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri", "file://" + targetPath}); 
        QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri-dark", "file://" + targetPath}); 
    }
    else if (desktop.contains("kde") || desktop.contains("plasma")) {
        // Use a safer way to pass the script to qdbus
        QString script = QString("var allDesktops = desktops();"
                                 "for (i=0;i<allDesktops.length;i++) {"
                                 "    d = allDesktops[i];"
                                 "    d.wallpaperPlugin = \"org.kde.image\";"
                                 "    d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");"
                                 "    d.writeConfig(\"Image\", \"file://%1\");"
                                 "}").arg(targetPath);
        QProcess::startDetached("qdbus", {"org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell.evaluateScript", script});
    }
    else { 
        if (QProcess::startDetached("feh", {"--bg-scale", targetPath})) 
            emit operationSuccess("Wallpaper set (feh)"); 
        else 
            emit operationError("Could not set wallpaper (try installing feh)"); 
    }
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
