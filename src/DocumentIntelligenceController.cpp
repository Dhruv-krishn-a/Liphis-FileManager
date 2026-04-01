#include "DocumentIntelligenceController.hpp"
#include "ThreadPool.hpp"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSet>
#include <QStandardPaths>

#include <algorithm>
#include <cmath>

namespace {
static QString normalizedPath(const QString &path) {
    return QDir::cleanPath(path);
}

static bool startsWithInsensitive(const QString &a, const QString &prefix) {
    return a.toLower().startsWith(prefix.toLower());
}

static bool shouldSkipDirectory(const QString &dirName, const QString &absPath) {
    const QString n = dirName.toLower();
    static const QSet<QString> blocked = {
        ".git", "node_modules", ".next", ".cache", ".venv", "venv", "__pycache__",
        "dist", "build", "target", "out", ".idea", ".vscode", ".gradle"
    };
    if (blocked.contains(n)) return true;
    const QString p = absPath.toLower();
    return p.contains("/node_modules/") || p.contains("/.git/") || p.contains("/.next/");
}

static bool noisyGroupId(const QString &groupId) {
    const QString g = groupId.toLower();
    static const QSet<QString> noisy = {
        "readme", "license", "changelog", "index", "package", "package-lock",
        "yarn-lock", "pnpm-lock", "copying", "notice"
    };
    return noisy.contains(g);
}

static const int kMaxIndexedDocs = 8000;
}

DocumentIntelligenceController::DocumentIntelligenceController(QObject *parent)
    : QObject(parent)
{
    m_downloadsDebounceTimer.setSingleShot(true);
    m_downloadsDebounceTimer.setInterval(850);
    QObject::connect(&m_downloadsDebounceTimer, &QTimer::timeout, this, [this]() {
        scanDownloadsInbox();
    });

    m_healthRefreshTimer.setSingleShot(false);
    m_healthRefreshTimer.setInterval(5 * 60 * 1000);
    QObject::connect(&m_healthRefreshTimer, &QTimer::timeout, this, [this]() {
        scanDownloadsInbox();
        rebuildHealthReport();
        emit operationNotice("Document health refreshed", false);
    });
}

void DocumentIntelligenceController::setEnabled(bool value)
{
    if (m_enabled == value) return;
    m_enabled = value;
    emit enabledChanged();
    saveStateToDb();
}

QString DocumentIntelligenceController::downloadsPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

void DocumentIntelligenceController::initialize()
{
    if (m_initialized) return;
    m_initialized = true;
    ensureDatabase();
    loadStateFromDb();
    // Startup must stay fast: avoid expensive duplicate/health rebuild here.
    // Data will be recomputed on explicit scans and user actions.

    const QString downloads = downloadsPath();
    if (!downloads.isEmpty()) {
        if (!m_downloadsWatcher.directories().contains(downloads)) {
            m_downloadsWatcher.addPath(downloads);
        }
    }
    QObject::connect(&m_downloadsWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &) {
        m_downloadsDebounceTimer.start();
    });
    m_healthRefreshTimer.start();
}

void DocumentIntelligenceController::ensureInitialized()
{
    if (!m_initialized) initialize();
}

bool DocumentIntelligenceController::isDocumentPath(const QString &path) const
{
    static const QSet<QString> exts = {
        "doc", "docx", "odt", "rtf", "txt", "md", "pdf",
        "csv", "xls", "xlsx", "ppt", "pptx", "ods", "odp"
    };
    const QString ext = QFileInfo(path).suffix().toLower();
    return exts.contains(ext);
}

QString DocumentIntelligenceController::normalizeBaseKey(const QString &name) const
{
    QString base = name;
    const int dot = base.lastIndexOf('.');
    if (dot > 0) base = base.left(dot);
    base = base.toLower().trimmed();

    base.replace(QRegularExpression("\\s+"), " ");
    base.replace(QRegularExpression("\\(\\d+\\)$"), "");
    base.replace(QRegularExpression("\\s*-\\s*copy$"), "");
    base.replace(QRegularExpression("\\s*-\\s*final$"), "");
    base.replace(QRegularExpression("\\s+final$"), "");
    base.replace(QRegularExpression("\\s+latest$"), "");
    base.replace(QRegularExpression("[-_ ]v\\d+$"), "");
    base = base.trimmed();
    if (base.isEmpty()) base = "untitled";
    return base;
}

QString DocumentIntelligenceController::computeQuickHash(const QString &path, qlonglong size) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return QStringLiteral("nohash:%1").arg(size);

    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(QByteArray::number(size));

    const qint64 chunk = 64 * 1024;
    hash.addData(file.read(chunk));
    if (size > chunk) {
        file.seek(std::max<qint64>(0, size - chunk));
        hash.addData(file.read(chunk));
    }
    return QString::fromLatin1(hash.result().toHex());
}

QString DocumentIntelligenceController::computeFullHash(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return {};
    QCryptographicHash hash(QCryptographicHash::Sha256);
    while (!file.atEnd()) {
        const QByteArray chunk = file.read(256 * 1024);
        if (chunk.isEmpty()) break;
        hash.addData(chunk);
    }
    return QString::fromLatin1(hash.result().toHex());
}

QVariantMap DocumentIntelligenceController::docToMap(const DocItem &doc) const
{
    QVariantMap m;
    m["path"] = doc.path;
    m["name"] = doc.name;
    m["extension"] = doc.extension;
    m["baseKey"] = doc.baseKey;
    m["groupId"] = doc.groupId;
    m["status"] = doc.status;
    m["size"] = doc.size;
    m["mtime"] = doc.mtime;
    m["quickHash"] = doc.quickHash;
    return m;
}

void DocumentIntelligenceController::markBusy(bool value)
{
    if (m_busy == value) return;
    m_busy = value;
    emit busyChanged();
}

void DocumentIntelligenceController::scanFolder(const QString &path)
{
    ensureInitialized();
    if (!m_enabled) return;
    const QString root = normalizedPath(path);
    if (root.isEmpty()) return;
    QDir rootDir(root);
    if (!rootDir.exists()) {
        emit operationNotice("Document scan path does not exist", true);
        return;
    }

    markBusy(true);
    const QVariantMap statusSnapshot = m_statusByPath;
    QPointer<DocumentIntelligenceController> safeThis(this);
    ThreadPool::instance().submit([safeThis, root, statusSnapshot]() {
        if (!safeThis) return;

        QList<DocItem> docs;
        QVector<QString> pendingDirs;
        pendingDirs.append(root);
        static const int kMaxDocs = kMaxIndexedDocs;

        while (!pendingDirs.isEmpty() && docs.size() < kMaxDocs) {
            if (!safeThis) return;
            const QString currentDir = pendingDirs.takeLast();
            QDir dir(currentDir);
            const QFileInfoList entries = dir.entryInfoList(
                QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::Readable,
                QDir::DirsFirst | QDir::Name
            );

            for (const QFileInfo &fi : entries) {
                if (!safeThis) return;
                if (fi.isDir()) {
                    if (!shouldSkipDirectory(fi.fileName(), fi.absoluteFilePath())) {
                        pendingDirs.append(fi.absoluteFilePath());
                    }
                    continue;
                }

                const QString p = normalizedPath(fi.absoluteFilePath());
                if (!safeThis->isDocumentPath(p)) continue;
                DocItem doc;
                doc.path = p;
                doc.name = fi.fileName();
                doc.extension = fi.suffix().toLower();
                doc.baseKey = safeThis->normalizeBaseKey(doc.name);
                doc.groupId = doc.baseKey;
                doc.size = fi.size();
                doc.mtime = fi.lastModified().toSecsSinceEpoch();
                doc.quickHash = safeThis->computeQuickHash(doc.path, doc.size);
                doc.status = statusSnapshot.value(doc.path, "Draft").toString();
                docs.append(doc);

                if (docs.size() >= kMaxDocs) break;
            }
        }

        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, root, docs]() mutable {
                if (!safeThis) return;
                safeThis->m_docs = docs;
                safeThis->m_lastScanRoot = root;
                emit safeThis->lastScanRootChanged();
                safeThis->rebuildDerivedData();
                safeThis->saveStateToDb();
                safeThis->markBusy(false);
                emit safeThis->operationNotice(QString("Document intelligence indexed %1 files").arg(safeThis->m_docs.size()), false);
            }, Qt::QueuedConnection);
        }
    });
}

void DocumentIntelligenceController::scanDownloadsInbox()
{
    ensureInitialized();
    if (!m_enabled) return;

    const QString downloads = downloadsPath();
    if (downloads.isEmpty()) return;
    QDir dir(downloads);
    if (!dir.exists()) return;

    const QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::Hidden,
        QDir::Time
    );

    QVariantList inbox = m_inboxItems;
    QSet<QString> existing;
    for (const QVariant &v : inbox) {
        existing.insert(v.toMap().value("path").toString());
    }

    for (const QFileInfo &fi : entries) {
        const QString p = normalizedPath(fi.absoluteFilePath());
        if (!isDocumentPath(p)) continue;

        const QString seenKey = p + "::" + QString::number(fi.lastModified().toSecsSinceEpoch());
        if (m_seenDownloads.contains(seenKey)) continue;

        QVariantMap item;
        const QString base = normalizeBaseKey(fi.fileName());
        item["path"] = p;
        item["name"] = fi.fileName();
        item["size"] = fi.size();
        item["mtime"] = fi.lastModified().toSecsSinceEpoch();
        item["baseKey"] = base;
        item["source"] = "download";
        item["suggestion"] = "keep";
        item["matchedGroup"] = "";
        item["confidence"] = 0;

        // Google-export style heuristics
        const bool looksVersioned = fi.fileName().contains(QRegularExpression("\\(\\d+\\)|copy|final|edited|google docs", QRegularExpression::CaseInsensitiveOption));
        for (const QVariant &gVar : m_groups) {
            const QVariantMap g = gVar.toMap();
            if (g.value("groupId").toString() == base || startsWithInsensitive(base, g.value("groupId").toString())) {
                item["matchedGroup"] = g.value("groupId");
                item["suggestion"] = looksVersioned ? "version" : "replace";
                item["confidence"] = looksVersioned ? 82 : 72;
                item["source"] = fi.fileName().contains(QRegularExpression("google docs|gdoc", QRegularExpression::CaseInsensitiveOption))
                    ? "google_export"
                    : "download_update";
                break;
            }
        }

        if (!existing.contains(p)) {
            inbox.append(item);
            existing.insert(p);
        }
        m_seenDownloads.insert(seenKey, true);
    }

    if (m_inboxItems != inbox) {
        m_inboxItems = inbox;
        emit inboxItemsChanged();
        saveStateToDb();
    }
}

QVariantMap DocumentIntelligenceController::groupForPath(const QString &path) const
{
    const QString normalized = normalizedPath(path);
    for (const QVariant &v : m_groups) {
        const QVariantMap g = v.toMap();
        const QVariantList versions = g.value("versions").toList();
        for (const QVariant &vv : versions) {
            if (vv.toMap().value("path").toString() == normalized) {
                return g;
            }
        }
    }
    return {};
}

QVariantMap DocumentIntelligenceController::timelineForPath(const QString &path) const
{
    return groupForPath(path);
}

void DocumentIntelligenceController::setCurrentVersion(const QString &groupId, const QString &path)
{
    ensureInitialized();
    if (groupId.isEmpty() || path.isEmpty()) {
        emit operationNotice("Set current failed: missing group or path", true);
        return;
    }

    const QString normalized = normalizedPath(path);
    bool foundInGroup = false;
    m_currentByGroup.insert(groupId, normalized);
    for (DocItem &doc : m_docs) {
        if (doc.groupId != groupId) continue;
        if (doc.path == normalized) {
            doc.status = "Current";
            foundInGroup = true;
        }
        else if (doc.status == "Current") doc.status = "Archived";
        m_statusByPath.insert(doc.path, doc.status);
    }
    if (!foundInGroup) {
        emit operationNotice("Set current failed: file not found in indexed group", true);
        return;
    }
    rebuildDerivedData();
    saveStateToDb();
    emit operationNotice("Marked as current version", false);
}

void DocumentIntelligenceController::setDocumentStatus(const QString &path, const QString &status)
{
    ensureInitialized();
    const QString normalized = normalizedPath(path);
    QString s = status.trimmed();
    if (s.isEmpty()) {
        emit operationNotice("Status update failed: empty status", true);
        return;
    }
    if (s != "Draft" && s != "Current" && s != "Final" && s != "Archived") {
        emit operationNotice("Status update failed: invalid status", true);
        return;
    }

    m_statusByPath.insert(normalized, s);
    QString targetGroup;
    bool found = false;
    for (DocItem &doc : m_docs) {
        if (doc.path == normalized) {
            doc.status = s;
            targetGroup = doc.groupId;
            if (s == "Current") m_currentByGroup.insert(doc.groupId, doc.path);
            found = true;
            break;
        }
    }
    if (!found) {
        emit operationNotice("File is not indexed yet. Click Scan Folder first.", true);
        return;
    }

    if (s == "Current" && !targetGroup.isEmpty()) {
        for (DocItem &doc : m_docs) {
            if (doc.groupId != targetGroup) continue;
            if (doc.path == normalized) continue;
            if (doc.status == "Current") {
                doc.status = "Archived";
                m_statusByPath.insert(doc.path, "Archived");
            }
        }
    }

    rebuildDerivedData();
    saveStateToDb();
    emit operationNotice(QString("Status set to %1").arg(s), false);
}

void DocumentIntelligenceController::triageInbox(const QString &path, const QString &action)
{
    ensureInitialized();
    const QString normalized = normalizedPath(path);
    int removeIndex = -1;
    QVariantMap inboxItem;
    for (int i = 0; i < m_inboxItems.size(); ++i) {
        QVariantMap item = m_inboxItems[i].toMap();
        if (item.value("path").toString() == normalized) {
            inboxItem = item;
            removeIndex = i;
            break;
        }
    }
    if (removeIndex < 0) return;

    const QString groupId = inboxItem.value("matchedGroup").toString();
    if (action == "replace" && !groupId.isEmpty()) {
        setCurrentVersion(groupId, normalized);
    } else if (action == "version" && !groupId.isEmpty()) {
        m_statusByPath.insert(normalized, "Draft");
    } else if (action == "ignore") {
        m_seenDownloads.insert(normalized + "::ignored", true);
    }

    m_inboxItems.removeAt(removeIndex);
    emit inboxItemsChanged();
    rebuildDerivedData();
    saveStateToDb();
}

void DocumentIntelligenceController::clearInbox()
{
    ensureInitialized();
    if (m_inboxItems.isEmpty()) return;
    m_inboxItems.clear();
    emit inboxItemsChanged();
    saveStateToDb();
}

QString DocumentIntelligenceController::buildCanonicalName(const DocItem &doc, const QString &nameTemplate, int versionIndex) const
{
    QString templ = nameTemplate.isEmpty() ? defaultNameTemplate() : nameTemplate;
    QString result = templ;
    const QString date = QDateTime::fromSecsSinceEpoch(doc.mtime).toString("yyyy-MM-dd");
    result.replace("{base}", doc.baseKey);
    result.replace("{date}", date);
    result.replace("{version}", QString("v%1").arg(versionIndex, 2, 10, QChar('0')));
    result.replace("{ext}", doc.extension);
    result.replace(QRegularExpression("[^a-zA-Z0-9._-]+"), "_");
    if (!result.endsWith("." + doc.extension)) result += "." + doc.extension;
    return result;
}

QVariantList DocumentIntelligenceController::previewCanonicalNames(const QString &rootPath, const QString &nameTemplate)
{
    ensureInitialized();
    QVariantList preview;
    const QString root = normalizedPath(rootPath);

    QMap<QString, QList<DocItem>> groups;
    for (const DocItem &doc : m_docs) {
        if (!root.isEmpty() && !doc.path.startsWith(root)) continue;
        groups[doc.groupId].append(doc);
    }

    for (auto it = groups.begin(); it != groups.end(); ++it) {
        QList<DocItem> docs = it.value();
        std::sort(docs.begin(), docs.end(), [](const DocItem &a, const DocItem &b) {
            return a.mtime < b.mtime;
        });
        int version = 1;
        for (const DocItem &doc : docs) {
            const QString newName = buildCanonicalName(doc, nameTemplate, version++);
            const QString newPath = QFileInfo(doc.path).dir().absoluteFilePath(newName);
            QVariantMap row;
            row["path"] = doc.path;
            row["oldName"] = doc.name;
            row["newName"] = newName;
            row["newPath"] = newPath;
            row["changed"] = (doc.path != newPath);
            row["conflict"] = QFileInfo::exists(newPath) && normalizedPath(newPath) != normalizedPath(doc.path);
            preview.append(row);
        }
    }

    m_namingPreview = preview;
    emit namingPreviewChanged();
    return preview;
}

int DocumentIntelligenceController::applyCanonicalNames()
{
    ensureInitialized();
    QVector<QPair<QString, QString>> ops;
    QSet<QString> fromSet;
    QSet<QString> toSet;
    for (const QVariant &v : m_namingPreview) {
        const QVariantMap row = v.toMap();
        if (!row.value("changed").toBool()) continue;
        if (row.value("conflict").toBool()) {
            emit operationNotice("Rename aborted: conflict detected in preview", true);
            return 0;
        }
        const QString from = row.value("path").toString();
        const QString to = row.value("newPath").toString();
        if (from.isEmpty() || to.isEmpty()) continue;
        ops.push_back({from, to});
        fromSet.insert(from);
        if (toSet.contains(to)) {
            emit operationNotice("Rename aborted: duplicate target names in preview", true);
            return 0;
        }
        toSet.insert(to);
    }

    for (const auto &op : ops) {
        if (QFileInfo::exists(op.second) && !fromSet.contains(op.second)) {
            emit operationNotice("Rename aborted: destination file already exists", true);
            return 0;
        }
    }

    QVariantList undoOps;
    QVector<QPair<QString, QString>> tempOps;
    int changed = 0;

    // Phase 1: move all sources to temp names
    for (int i = 0; i < ops.size(); ++i) {
        const QString from = ops[i].first;
        const QString temp = from + ".liphis_tmp_rename_" + QString::number(i);
        if (!QFile::rename(from, temp)) {
            // rollback temp phase
            for (const auto &t : tempOps) QFile::rename(t.second, t.first);
            emit operationNotice("Rename aborted during temporary staging", true);
            return 0;
        }
        tempOps.push_back({from, temp});
    }

    // Phase 2: move temp names to final targets
    for (int i = 0; i < ops.size(); ++i) {
        const QString to = ops[i].second;
        const QString temp = tempOps[i].second;
        if (!QFile::rename(temp, to)) {
            // rollback finals that already moved
            for (int j = 0; j < i; ++j) {
                QFile::rename(ops[j].second, tempOps[j].first);
            }
            // rollback remaining temp staged
            for (int j = i; j < tempOps.size(); ++j) {
                QFile::rename(tempOps[j].second, tempOps[j].first);
            }
            emit operationNotice("Rename aborted during final move", true);
            return 0;
        }
        QVariantMap undo;
        undo["from"] = ops[i].first;
        undo["to"] = ops[i].second;
        undoOps.append(undo);
        changed++;
    }

    m_lastRenameUndo = undoOps;
    if (changed > 0 && !m_lastScanRoot.isEmpty()) scanFolder(m_lastScanRoot);
    emit operationNotice(QString("Canonical rename applied to %1 files").arg(changed), false);
    return changed;
}

int DocumentIntelligenceController::undoLastCanonicalRename()
{
    ensureInitialized();
    int changed = 0;
    for (int i = m_lastRenameUndo.size() - 1; i >= 0; --i) {
        const QVariantMap op = m_lastRenameUndo[i].toMap();
        const QString from = op.value("from").toString();
        const QString to = op.value("to").toString();
        if (from.isEmpty() || to.isEmpty()) continue;
        if (QFile::exists(to) && QFile::rename(to, from)) changed++;
    }
    m_lastRenameUndo.clear();
    if (changed > 0 && !m_lastScanRoot.isEmpty()) scanFolder(m_lastScanRoot);
    emit operationNotice(QString("Undo rename restored %1 files").arg(changed), false);
    return changed;
}

void DocumentIntelligenceController::mergeExactDuplicateGroup(const QString &hashKey, const QString &action)
{
    ensureInitialized();
    if (hashKey.isEmpty()) return;
    QVariantMap target;
    for (const QVariant &v : m_duplicateExact) {
        const QVariantMap m = v.toMap();
        if (m.value("hashKey").toString() == hashKey) {
            target = m;
            break;
        }
    }
    if (target.isEmpty()) return;

    const QVariantList paths = target.value("paths").toList();
    if (paths.size() < 2) return;

    if (action == "archive") {
        const QString keeper = paths.first().toString();
        for (const QVariant &p : paths) {
            const QString path = p.toString();
            if (path == keeper) continue;
            m_statusByPath.insert(path, "Archived");
        }
    } else if (action == "keep_latest") {
        QString latest;
        qlonglong bestTime = -1;
        for (const QVariant &p : paths) {
            const QString path = p.toString();
            QFileInfo fi(path);
            const qlonglong mtime = fi.lastModified().toSecsSinceEpoch();
            if (mtime > bestTime) {
                bestTime = mtime;
                latest = path;
            }
        }
        for (const QVariant &p : paths) {
            const QString path = p.toString();
            if (path == latest) continue;
            m_statusByPath.insert(path, "Archived");
        }
    } else if (action == "ignore_set") {
        m_ignoredDuplicateHashes.insert(hashKey, true);
    }
    rebuildDerivedData();
    saveStateToDb();
}

void DocumentIntelligenceController::attachStatusesToGroups()
{
    for (QVariant &v : m_groups) {
        QVariantMap g = v.toMap();
        QVariantList versions = g.value("versions").toList();
        for (QVariant &vv : versions) {
            QVariantMap doc = vv.toMap();
            doc["status"] = m_statusByPath.value(doc.value("path").toString(), "Draft").toString();
            vv = doc;
        }
        g["versions"] = versions;
        v = g;
    }
}

void DocumentIntelligenceController::rebuildGroupsAndDuplicates()
{
    QMap<QString, QList<DocItem>> byGroup;
    for (const DocItem &doc : m_docs) byGroup[doc.groupId].append(doc);

    QVariantList groups;
    QVariantList exact;
    QVariantList near;
    static const int kMaxExactGroups = 800;
    static const int kMaxNearPairs = 6000;
    static const int kMaxGroupNearCompare = 70;
    static const int kMaxFullHashFiles = 900;
    int fullHashFiles = 0;

    // Exact duplicates grouped by hash+size
    QMap<QString, QStringList> byExactHash;
    for (const DocItem &doc : m_docs) {
        const QString key = doc.quickHash + "::" + QString::number(doc.size);
        byExactHash[key].append(doc.path);
    }
    for (auto it = byExactHash.begin(); it != byExactHash.end(); ++it) {
        if (it.value().size() < 2) continue;
        if (exact.size() >= kMaxExactGroups) break;
        QMap<QString, QStringList> byFull;
        for (const QString &path : it.value()) {
            if (fullHashFiles >= kMaxFullHashFiles) break;
            const QString full = computeFullHash(path);
            if (!full.isEmpty()) {
                byFull[full].append(path);
                fullHashFiles++;
            }
        }
        for (auto fit = byFull.begin(); fit != byFull.end(); ++fit) {
            if (fit.value().size() < 2) continue;
            const QString hashKey = fit.key() + "::" + QString::number(QFileInfo(fit.value().first()).size());
            if (m_ignoredDuplicateHashes.contains(hashKey)) continue;
            QVariantMap m;
            m["hashKey"] = hashKey;
            m["paths"] = fit.value();
            m["count"] = fit.value().size();
            exact.append(m);
            if (exact.size() >= kMaxExactGroups) break;
        }
    }

    for (auto it = byGroup.begin(); it != byGroup.end(); ++it) {
        QList<DocItem> docs = it.value();
        std::sort(docs.begin(), docs.end(), [](const DocItem &a, const DocItem &b) {
            return a.mtime > b.mtime;
        });

        const QString groupId = it.key();
        QVariantMap group;
        group["groupId"] = groupId;
        group["canonicalName"] = groupId;
        group["versionCount"] = docs.size();

        QVariantList versions;
        for (const DocItem &doc : docs) versions.append(docToMap(doc));
        group["versions"] = versions;

        QString current = m_currentByGroup.value(groupId).toString();
        if (current.isEmpty() && !docs.isEmpty()) current = docs.first().path;
        group["currentPath"] = current;
        group["latestPath"] = docs.isEmpty() ? "" : docs.first().path;
        groups.append(group);

        // Near duplicates in same group
        if (near.size() >= kMaxNearPairs) continue;
        if (noisyGroupId(groupId) && docs.size() > 20) continue;

        const int compareCount = std::min<int>(docs.size(), kMaxGroupNearCompare);
        for (int i = 0; i < compareCount; ++i) {
            for (int j = i + 1; j < compareCount; ++j) {
                if (docs[i].quickHash == docs[j].quickHash) continue;
                const double maxSize = static_cast<double>(std::max<qlonglong>(1, std::max(docs[i].size, docs[j].size)));
                const double diff = std::abs(static_cast<double>(docs[i].size - docs[j].size)) / maxSize;
                int score = static_cast<int>(std::round(100.0 - (diff * 100.0)));
                if (score < 70) continue;
                QVariantMap n;
                n["groupId"] = groupId;
                n["pathA"] = docs[i].path;
                n["pathB"] = docs[j].path;
                n["nameA"] = docs[i].name;
                n["nameB"] = docs[j].name;
                n["score"] = score;
                near.append(n);
                if (near.size() >= kMaxNearPairs) break;
            }
            if (near.size() >= kMaxNearPairs) break;
        }
    }

    m_groups = groups;
    m_duplicateExact = exact;
    m_duplicateNear = near;
    attachStatusesToGroups();
    emit groupsChanged();
    emit duplicatesChanged();
}

void DocumentIntelligenceController::rebuildHealthReport()
{
    int draftCount = 0;
    int archivedCount = 0;
    int finalCount = 0;
    for (const DocItem &doc : m_docs) {
        const QString status = m_statusByPath.value(doc.path, "Draft").toString();
        if (status == "Draft") draftCount++;
        else if (status == "Archived") archivedCount++;
        else if (status == "Final") finalCount++;
    }

    const int duplicateBurden = m_duplicateExact.size() + m_duplicateNear.size();
    int score = 100;
    score -= std::min(35, duplicateBurden * 4);
    score -= std::min(25, draftCount / 3);
    score += std::min(10, finalCount / 2);
    score = std::clamp(score, 0, 100);

    QVariantMap health;
    health["score"] = score;
    health["totalDocs"] = m_docs.size();
    health["groups"] = m_groups.size();
    health["exactDuplicates"] = m_duplicateExact.size();
    health["nearDuplicates"] = m_duplicateNear.size();
    health["drafts"] = draftCount;
    health["archived"] = archivedCount;
    health["finals"] = finalCount;
    health["inbox"] = m_inboxItems.size();
    health["label"] = score >= 85 ? "Healthy" : (score >= 60 ? "Needs attention" : "At risk");

    m_healthReport = health;
    emit healthReportChanged();
}

void DocumentIntelligenceController::rebuildDerivedData()
{
    rebuildGroupsAndDuplicates();
    rebuildHealthReport();
}

QString DocumentIntelligenceController::statePath() const
{
    const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(baseDir);
    return baseDir + "/doc_intel_state.json";
}

void DocumentIntelligenceController::loadState()
{
    QFile file(statePath());
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly)) return;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return;
    const QJsonObject root = doc.object();
    m_enabled = root.value("enabled").toBool(true);
    m_lastScanRoot = root.value("lastScanRoot").toString();
    m_statusByPath = root.value("statusByPath").toObject().toVariantMap();
    m_currentByGroup = root.value("currentByGroup").toObject().toVariantMap();
    m_seenDownloads = root.value("seenDownloads").toObject().toVariantMap();
    m_ignoredDuplicateHashes = root.value("ignoredDuplicateHashes").toObject().toVariantMap();
    m_inboxItems = root.value("inboxItems").toArray().toVariantList();
}

void DocumentIntelligenceController::saveState() const
{
    QFile file(statePath());
    if (!file.open(QIODevice::WriteOnly)) return;
    QJsonObject root;
    root["enabled"] = m_enabled;
    root["lastScanRoot"] = m_lastScanRoot;
    root["statusByPath"] = QJsonObject::fromVariantMap(m_statusByPath);
    root["currentByGroup"] = QJsonObject::fromVariantMap(m_currentByGroup);
    root["seenDownloads"] = QJsonObject::fromVariantMap(m_seenDownloads);
    root["ignoredDuplicateHashes"] = QJsonObject::fromVariantMap(m_ignoredDuplicateHashes);
    root["inboxItems"] = QJsonArray::fromVariantList(m_inboxItems);
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

QString DocumentIntelligenceController::dbPath() const
{
    const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(baseDir);
    return baseDir + "/doc_intel.sqlite";
}

bool DocumentIntelligenceController::ensureDatabase()
{
    QSqlDatabase db = QSqlDatabase::database("docintel", false);
    if (!db.isValid()) db = QSqlDatabase::addDatabase("QSQLITE", "docintel");
    db.setDatabaseName(dbPath());
    if (!db.open()) return false;

    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS meta (key TEXT PRIMARY KEY, value TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS docs ("
           "path TEXT PRIMARY KEY, name TEXT, extension TEXT, baseKey TEXT, groupId TEXT, "
           "status TEXT, size INTEGER, mtime INTEGER, quickHash TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS inbox (path TEXT PRIMARY KEY, payload TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS overrides (kind TEXT, key TEXT, value TEXT, PRIMARY KEY(kind, key))");
    return true;
}

void DocumentIntelligenceController::saveStateToDb() const
{
    QSqlDatabase db = QSqlDatabase::database("docintel", false);
    if (!db.isValid() || !db.isOpen()) return;
    db.transaction();
    QSqlQuery q(db);
    q.exec("DELETE FROM docs");
    q.exec("DELETE FROM inbox");
    q.exec("DELETE FROM overrides");
    q.exec("DELETE FROM meta");

    q.prepare("INSERT OR REPLACE INTO meta(key, value) VALUES(?, ?)");
    q.addBindValue("enabled"); q.addBindValue(m_enabled ? "1" : "0"); q.exec();
    q.addBindValue("lastScanRoot"); q.addBindValue(m_lastScanRoot); q.exec();

    QSqlQuery d(db);
    d.prepare("INSERT OR REPLACE INTO docs(path,name,extension,baseKey,groupId,status,size,mtime,quickHash) VALUES(?,?,?,?,?,?,?,?,?)");
    for (const DocItem &doc : m_docs) {
        d.addBindValue(doc.path);
        d.addBindValue(doc.name);
        d.addBindValue(doc.extension);
        d.addBindValue(doc.baseKey);
        d.addBindValue(doc.groupId);
        d.addBindValue(doc.status);
        d.addBindValue(doc.size);
        d.addBindValue(doc.mtime);
        d.addBindValue(doc.quickHash);
        d.exec();
    }

    QSqlQuery i(db);
    i.prepare("INSERT OR REPLACE INTO inbox(path, payload) VALUES(?, ?)");
    for (const QVariant &v : m_inboxItems) {
        const QVariantMap map = v.toMap();
        i.addBindValue(map.value("path").toString());
        i.addBindValue(QString::fromUtf8(QJsonDocument::fromVariant(map).toJson(QJsonDocument::Compact)));
        i.exec();
    }

    QSqlQuery o(db);
    o.prepare("INSERT OR REPLACE INTO overrides(kind, key, value) VALUES(?, ?, ?)");
    for (auto it = m_statusByPath.begin(); it != m_statusByPath.end(); ++it) {
        o.addBindValue("status");
        o.addBindValue(it.key());
        o.addBindValue(it.value().toString());
        o.exec();
    }
    for (auto it = m_currentByGroup.begin(); it != m_currentByGroup.end(); ++it) {
        o.addBindValue("current");
        o.addBindValue(it.key());
        o.addBindValue(it.value().toString());
        o.exec();
    }
    for (auto it = m_seenDownloads.begin(); it != m_seenDownloads.end(); ++it) {
        o.addBindValue("seen");
        o.addBindValue(it.key());
        o.addBindValue("1");
        o.exec();
    }
    for (auto it = m_ignoredDuplicateHashes.begin(); it != m_ignoredDuplicateHashes.end(); ++it) {
        o.addBindValue("ignore_hash");
        o.addBindValue(it.key());
        o.addBindValue("1");
        o.exec();
    }

    db.commit();
}

void DocumentIntelligenceController::loadStateFromDb()
{
    QSqlDatabase db = QSqlDatabase::database("docintel", false);
    if (!db.isValid() || !db.isOpen()) {
        loadState();
        return;
    }

    m_docs.clear();
    m_inboxItems.clear();
    m_statusByPath.clear();
    m_currentByGroup.clear();
    m_seenDownloads.clear();
    m_ignoredDuplicateHashes.clear();

    QSqlQuery q("SELECT key, value FROM meta", db);
    while (q.next()) {
        const QString key = q.value(0).toString();
        const QString value = q.value(1).toString();
        if (key == "enabled") m_enabled = (value == "1");
        else if (key == "lastScanRoot") m_lastScanRoot = value;
    }

    QSqlQuery d("SELECT path,name,extension,baseKey,groupId,status,size,mtime,quickHash FROM docs", db);
    while (d.next()) {
        DocItem doc;
        doc.path = d.value(0).toString();
        const QFileInfo dfi(doc.path);
        if (shouldSkipDirectory(dfi.dir().dirName(), doc.path)) continue;
        if (!isDocumentPath(doc.path)) continue;
        doc.name = d.value(1).toString();
        doc.extension = d.value(2).toString();
        doc.baseKey = d.value(3).toString();
        doc.groupId = d.value(4).toString();
        doc.status = d.value(5).toString();
        doc.size = d.value(6).toLongLong();
        doc.mtime = d.value(7).toLongLong();
        doc.quickHash = d.value(8).toString();
        m_docs.append(doc);
        if (m_docs.size() >= kMaxIndexedDocs) break;
    }

    QSqlQuery i("SELECT payload FROM inbox", db);
    while (i.next()) {
        const QString payload = i.value(0).toString();
        const QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
        if (doc.isObject()) m_inboxItems.append(doc.object().toVariantMap());
    }

    QSqlQuery o("SELECT kind, key, value FROM overrides", db);
    while (o.next()) {
        const QString kind = o.value(0).toString();
        const QString key = o.value(1).toString();
        const QString value = o.value(2).toString();
        if (kind == "status") m_statusByPath.insert(key, value);
        else if (kind == "current") m_currentByGroup.insert(key, value);
        else if (kind == "seen") m_seenDownloads.insert(key, true);
        else if (kind == "ignore_hash") m_ignoredDuplicateHashes.insert(key, true);
    }
}
