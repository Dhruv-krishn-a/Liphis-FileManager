#include "FileListModel.hpp"

#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDateTime>
#include <unordered_map>
#include <mutex>

namespace {
    std::unordered_map<std::string, std::shared_ptr<const std::string>> g_mimePool;
    std::mutex g_mimeMutex;

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
    if (size == 0) return "-";
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
        return entry.isDir ? "-" : formatSize(entry.size);
    case IsDirRole:
        return entry.isDir;
    case MTimeRole:
        return static_cast<qlonglong>(entry.mtime);
    case FormattedDateRole:
        return formatDate(entry.mtime);
    case CTimeRole:
        return static_cast<qlonglong>(entry.ctime);
    case ATimeRole:
        return static_cast<qlonglong>(entry.atime);
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
        return db.mimeTypeForName(entry.mimeType ? QString::fromStdString(*entry.mimeType) : "application/octet-stream").iconName();
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
        {ATimeRole, "atime"},
        {PermissionsRole, "permissions"},
        {ModeRole, "mode"},
        {OwnerRole, "owner"},
        {GroupRole, "group"},
        {ThumbnailRole, "thumbnail"},
        {IconNameRole, "iconName"}
    };
}

void FileListModel::clear()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
    emit countChanged();
}

void FileListModel::setEntries(std::vector<FileMeta> &&entries)
{
    static QMimeDatabase db;
    for (auto &item : entries) {
        if (item.isDir) item.mimeType = internMime("inode/directory");
        else item.mimeType = internMime(db.mimeTypeForFile(QString::fromStdString(item.path)).name().toStdString());
    }
    beginResetModel();
    m_entries = std::move(entries);
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
            item.mimeType = internMime(db.mimeTypeForFile(QString::fromStdString(item.path)).name().toStdString());
        }
    }

    const int startRow = static_cast<int>(m_entries.size());
    const int endRow = startRow + static_cast<int>(batch.size()) - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);

    for (auto &item : batch)
        m_entries.emplace_back(std::move(item));

    endInsertRows();
    emit countChanged();
}

void FileListModel::updateThumbnail(const QString &filePath,
                                    const QString &thumbPath)
{
    for (size_t i = 0; i < m_entries.size(); ++i) {
        if (QString::fromStdString(m_entries[i].path) == filePath) {
            m_entries[i].thumbnailPath = thumbPath.toStdString();
            QModelIndex idx = index(static_cast<int>(i));
            emit dataChanged(idx, idx, {ThumbnailRole});
            break;
        }
    }
}

QVariantMap FileListModel::metadataForPath(const QString &path) const
{
    for (const auto &entry : m_entries) {
        if (QString::fromStdString(entry.path) == path) {
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
