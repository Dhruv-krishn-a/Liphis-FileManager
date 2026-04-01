#include "FileListModel.hpp"

#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDateTime>
#include <algorithm>
#include <unordered_map>
#include <mutex>

namespace {
    std::unordered_map<std::string, std::shared_ptr<const std::string>> g_mimePool;
    std::mutex g_mimeMutex;
    
    std::unordered_map<std::string, std::shared_ptr<const std::string>> g_extMimeCache;
    std::mutex g_extMimeMutex;

    std::shared_ptr<const std::string> internMime(const std::string& s) {
        std::lock_guard<std::mutex> lock(g_mimeMutex);
        auto it = g_mimePool.find(s);
        if (it != g_mimePool.end()) return it->second;
        auto shared = std::make_shared<const std::string>(s);
        g_mimePool[s] = shared;
        return shared;
    }
}

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(m_entries.size());
}

static bool isSupportedImage(const QString &path) {
    static const QStringList imageExt = { "png","jpg","jpeg","bmp","webp","gif" };
    QFileInfo fi(path);
    return imageExt.contains(fi.suffix().toLower());
}

static QString formatSize(std::uint64_t size) {
    if (size == 0) return "0 B";
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double s = static_cast<double>(size);
    while (s >= 1024 && unit < 4) {
        s /= 1024;
        unit++;
    }
    return QString::number(s, 'f', unit > 0 ? 1 : 0) + " " + units[unit];
}

static QString formatDate(std::uint64_t epoch) {
    if (epoch == 0) return "-";
    return QDateTime::fromSecsSinceEpoch(epoch).toString("MMM d, yyyy HH:mm");
}

void FileListModel::setSortBy(const QString &field, bool ascending)
{
    if (m_sortField == field && m_sortAscending == ascending) return;
    m_sortField = field;
    m_sortAscending = ascending;
    applySort();
}

void FileListModel::applySort()
{
    if (m_entries.empty()) return;

    std::sort(m_entries.begin(), m_entries.end(), [this](const FileMeta &a, const FileMeta &b) {
        // Folders always first - this logic should NOT be inverted by m_sortAscending
        if (a.isDir != b.isDir) return a.isDir;

        bool result = false;
        bool equal = false;

        if (m_sortField == "size") {
            result = a.size < b.size;
            equal = (a.size == b.size);
        }
        else if (m_sortField == "date" || m_sortField == "mtime") {
            result = a.mtime < b.mtime;
            equal = (a.mtime == b.mtime);
        }
        else if (m_sortField == "ctime") {
            result = a.ctime < b.ctime;
            equal = (a.ctime == b.ctime);
        }
        else if (m_sortField == "atime") {
            result = a.atime < b.atime;
            equal = (a.atime == b.atime);
        }
        else if (m_sortField == "type") {
            QString typeA = a.isDir ? "Folder" : (a.mimeType ? QString::fromStdString(*a.mimeType) : "");
            QString typeB = b.isDir ? "Folder" : (b.mimeType ? QString::fromStdString(*b.mimeType) : "");
            result = typeA.compare(typeB, Qt::CaseInsensitive) < 0;
            equal = (typeA.compare(typeB, Qt::CaseInsensitive) == 0);
        }
        else if (m_sortField == "permissions") {
            QString pA = a.permissions ? QString::fromStdString(*a.permissions) : "";
            QString pB = b.permissions ? QString::fromStdString(*b.permissions) : "";
            result = pA < pB;
            equal = (pA == pB);
        }
        else if (m_sortField == "owner") {
            QString oA = a.owner ? QString::fromStdString(*a.owner) : "";
            QString oB = b.owner ? QString::fromStdString(*b.owner) : "";
            result = oA < oB;
            equal = (oA == oB);
        }
        else { // Default: name
            equal = true; // Fall through to name sort
        }

        if (equal) {
            result = QString::fromStdString(a.name).compare(QString::fromStdString(b.name), Qt::CaseInsensitive) < 0;
        }

        return m_sortAscending ? result : !result;
    });

    rebuildPathMap();
    beginResetModel();
    endResetModel();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_entries.size()))
        return {};

    const FileMeta &entry = m_entries[index.row()];
    QString path = QString::fromStdString(entry.path);

    switch (role) {
    case NameRole:
        return QString::fromStdString(entry.name);
    case PathRole:
        return path;
    case SizeRole:
        return static_cast<qlonglong>(entry.size);
    case FormattedSizeRole:
        if (entry.isDir) {
            return QString("%1 items").arg(entry.itemCount);
        }
        return formatSize(entry.size);
    case IsDirRole:
        return entry.isDir;
    case MTimeRole:
        return static_cast<qlonglong>(entry.mtime);
    case FormattedDateRole:
        return formatDate(entry.mtime);
    case CTimeRole:
        return static_cast<qlonglong>(entry.ctime);
    case FormattedCTimeRole:
        return formatDate(entry.ctime);
    case ATimeRole:
        return static_cast<qlonglong>(entry.atime);
    case FormattedATimeRole:
        return formatDate(entry.atime);
    case PermissionsRole:
        return entry.permissions ? QString::fromStdString(*entry.permissions) : "";
    case ModeRole:
        return static_cast<int>(entry.mode);
    case OwnerRole:
        return entry.owner ? QString::fromStdString(*entry.owner) : "";
    case GroupRole:
        return entry.group ? QString::fromStdString(*entry.group) : "";
    case ThumbnailRole: {
        if (isSupportedImage(path)) {
             return QString("image://thumbs/%1").arg(path);
        }
        return QString();
    }
    case IconNameRole: {
        if (entry.isDir) return "folder";
        static QMimeDatabase db;
        const QString m = entry.mimeType ? QString::fromStdString(*entry.mimeType) : "application/octet-stream";
        return db.mimeTypeForName(m).iconName();
    }
    case MimeTypeRole: {
        return entry.mimeType ? QString::fromStdString(*entry.mimeType) : "application/octet-stream";
    }
    case TypeRole: {
        if (entry.isDir) return "Folder";
        static QMimeDatabase db;
        const QString m = entry.mimeType ? QString::fromStdString(*entry.mimeType) : "application/octet-stream";
        return db.mimeTypeForName(m).comment();
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {SizeRole, "size"},
        {FormattedSizeRole, "formattedSize"},
        {IsDirRole, "isDir"},
        {MTimeRole, "mtime"},
        {FormattedDateRole, "formattedDate"},
        {CTimeRole, "ctime"},
        {FormattedCTimeRole, "formattedCTime"},
        {ATimeRole, "atime"},
        {FormattedATimeRole, "formattedATime"},
        {PermissionsRole, "permissions"},
        {ModeRole, "mode"},
        {OwnerRole, "owner"},
        {GroupRole, "group"},
        {ThumbnailRole, "thumbnail"},
        {IconNameRole, "iconName"},
        {MimeTypeRole, "mimeType"},
        {TypeRole, "type"}
    };
}

void FileListModel::clear()
{
    beginResetModel();
    m_entries.clear();
    m_pathToIndex.clear();
    endResetModel();
    emit countChanged();
}

void FileListModel::rebuildPathMap()
{
    m_pathToIndex.clear();
    for (size_t i = 0; i < m_entries.size(); ++i) {
        m_pathToIndex[m_entries[i].path] = i;
    }
}

void FileListModel::setEntries(std::vector<FileMeta> &&entries)
{
    static QMimeDatabase db;
    for (auto &item : entries) {
        if (item.isDir) {
            item.mimeType = internMime("inode/directory");
        } else {
            QFileInfo fi(QString::fromStdString(item.path));
            std::string ext = fi.suffix().toLower().toStdString();
            if (!ext.empty()) {
                std::lock_guard<std::mutex> lock(g_extMimeMutex);
                auto it = g_extMimeCache.find(ext);
                if (it != g_extMimeCache.end()) {
                    item.mimeType = it->second;
                } else {
                    auto mimeStr = db.mimeTypeForFile(fi.absoluteFilePath()).name().toStdString();
                    item.mimeType = internMime(mimeStr);
                    g_extMimeCache[ext] = item.mimeType;
                }
            } else {
                item.mimeType = internMime(db.mimeTypeForFile(fi.absoluteFilePath()).name().toStdString());
            }
        }
    }
    beginResetModel();
    m_entries = std::move(entries);
    applySort();
    rebuildPathMap();
    endResetModel();
    emit countChanged();
}

void FileListModel::insertBatch(std::vector<FileMeta> &&batch)
{
    if (batch.empty())
        return;

    static QMimeDatabase db;
    for (auto &item : batch) {
        if (item.isDir) {
            item.mimeType = internMime("inode/directory");
        } else {
            QFileInfo fi(QString::fromStdString(item.path));
            std::string ext = fi.suffix().toLower().toStdString();
            if (!ext.empty()) {
                std::lock_guard<std::mutex> lock(g_extMimeMutex);
                auto it = g_extMimeCache.find(ext);
                if (it != g_extMimeCache.end()) {
                    item.mimeType = it->second;
                } else {
                    auto mimeStr = db.mimeTypeForFile(fi.absoluteFilePath()).name().toStdString();
                    item.mimeType = internMime(mimeStr);
                    g_extMimeCache[ext] = item.mimeType;
                }
            } else {
                item.mimeType = internMime(db.mimeTypeForFile(fi.absoluteFilePath()).name().toStdString());
            }
        }
    }

    const int startRow = static_cast<int>(m_entries.size());
    const int endRow = startRow + static_cast<int>(batch.size()) - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);

    for (auto &item : batch) {
        m_pathToIndex[item.path] = m_entries.size();
        m_entries.emplace_back(std::move(item));
    }

    endInsertRows();
    emit countChanged();
}

void FileListModel::updateThumbnail(const QString &filePath,
                                    const QString &thumbPath)
{
    auto it = m_pathToIndex.find(filePath.toStdString());
    if (it != m_pathToIndex.end()) {
        size_t i = it->second;
        m_entries[i].thumbnailPath = thumbPath.toStdString();
        QModelIndex idx = index(static_cast<int>(i));
        emit dataChanged(idx, idx, {ThumbnailRole});
    }
}

QVariantMap FileListModel::metadataForPath(const QString &path) const
{
    auto it = m_pathToIndex.find(path.toStdString());

    if (it != m_pathToIndex.end()) {
        const auto &entry = m_entries[it->second];
        QVariantMap m;
        m["name"] = QString::fromStdString(entry.name);
        m["path"] = QString::fromStdString(entry.path);
        m["size"] = static_cast<qlonglong>(entry.size);
        m["isDir"] = entry.isDir;
        m["mtime"] = static_cast<qlonglong>(entry.mtime);
        m["ctime"] = static_cast<qlonglong>(entry.ctime);
        m["atime"] = static_cast<qlonglong>(entry.atime);
        m["permissions"] = entry.permissions ? QString::fromStdString(*entry.permissions) : "";
        m["mode"] = static_cast<int>(entry.mode);
        m["owner"] = entry.owner ? QString::fromStdString(*entry.owner) : "";
        m["group"] = entry.group ? QString::fromStdString(*entry.group) : "";
        m["mimeType"] = entry.mimeType ? QString::fromStdString(*entry.mimeType) : "application/octet-stream";
        if (entry.isDir) {
            m["iconName"] = "folder";
        } else {
            static QMimeDatabase db;
            m["iconName"] = db.mimeTypeForName(m["mimeType"].toString()).iconName();
        }

        if (!entry.isDir && isSupportedImage(path))
            m["thumbnail"] = QString("image://thumbs/%1").arg(path);
        else {
            m["thumbnail"] = QString();
        }
        return m;
    }
    return {};
}

QStringList FileListModel::availableExtensions() const
{
    QSet<QString> exts;
    for (const auto& entry : m_entries) {
        if (!entry.isDir) {
            QString name = QString::fromStdString(entry.name);
            int dotIdx = name.lastIndexOf('.');
            if (dotIdx > 0 && dotIdx < name.length() - 1) {
                QString ext = name.mid(dotIdx + 1).toLower();
                bool isPureNumeric = true;
                for(int i=0; i<ext.length(); ++i) {
                    if(!ext[i].isDigit()) {
                        isPureNumeric = false;
                        break;
                    }
                }
                if (!isPureNumeric && ext.length() < 10) {
                    exts.insert(ext);
                }
            }
        }
    }
    QStringList list = exts.values();
    list.sort();
    return list;
}
