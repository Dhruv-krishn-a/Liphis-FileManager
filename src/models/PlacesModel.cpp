#include "PlacesModel.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QStorageInfo>
#include <QSettings>
#include <QUrl>
#include <QVariantMap>

namespace {
QString normalizePath(QString path) {
    path = path.trimmed();
    if (path.startsWith("file://")) {
        const QUrl u(path);
        if (u.isValid() && u.isLocalFile()) path = u.toLocalFile();
        else path.remove(0, QString("file://").size());
    }
    path = QDir::fromNativeSeparators(path);
    path = QDir::cleanPath(path);
    if (path.length() > 1 && path.endsWith('/')) path.chop(1);
    return path;
}
}

PlacesModel::PlacesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setupDefaultPlaces();
}

int PlacesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_items.size());
}

QVariant PlacesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size()))
        return {};

    const auto &item = m_items[index.row()];

    switch (role) {
    case NameRole: return item.name;
    case PathRole: return item.path;
    case IconRole: return item.icon;
    case CategoryRole: return item.category;
    default: return {};
    }
}

QHash<int, QByteArray> PlacesModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {IconRole, "icon"},
        {CategoryRole, "category"}
    };
}

void PlacesModel::refresh()
{
    beginResetModel();
    setupDefaultPlaces();
    endResetModel();
}

void PlacesModel::addBookmark(const QString &path, const QString &name)
{
    const QString normalized = normalizePath(path);
    if (normalized.isEmpty()) return;
    QSettings settings("Liphis", "Bookmarks");
    QVariantList list = settings.value("places").toList();

    for (const QVariant &entry : list) {
        if (normalizePath(entry.toMap().value("path").toString()) == normalized) {
            return;
        }
    }

    QVariantMap item;
    item["name"] = name;
    item["path"] = normalized;
    list.append(item);

    settings.setValue("places", list);
    refresh();
}

void PlacesModel::removeBookmark(int index)
{
    if (index < 0 || index >= m_items.size()) return;
    if (m_items[index].category != 1) return;

    QSettings settings("Liphis", "Bookmarks");
    QVariantList list = settings.value("places").toList();
    
    // Find matching path in list
    const QString path = normalizePath(m_items[index].path);
    for (int i=0; i<list.size(); ++i) {
        if (normalizePath(list[i].toMap()["path"].toString()) == path) {
            list.removeAt(i);
            break;
        }
    }
    
    settings.setValue("places", list);
    refresh();
}

void PlacesModel::removeBookmarkByPath(const QString &path)
{
    const QString normalized = normalizePath(path);
    if (normalized.isEmpty()) return;
    QSettings settings("Liphis", "Bookmarks");
    QVariantList list = settings.value("places").toList();
    for (int i = 0; i < list.size(); ++i) {
        if (normalizePath(list[i].toMap().value("path").toString()) == normalized) {
            list.removeAt(i);
            settings.setValue("places", list);
            refresh();
            return;
        }
    }
}

bool PlacesModel::isBookmarked(const QString &path) const
{
    const QString normalized = normalizePath(path);
    if (normalized.isEmpty()) return false;
    QSettings settings("Liphis", "Bookmarks");
    const QVariantList list = settings.value("places").toList();
    for (const QVariant &entry : list) {
        if (normalizePath(entry.toMap().value("path").toString()) == normalized) {
            return true;
        }
    }
    return false;
}

void PlacesModel::addRecent(const QString &path)
{
    QSettings settings("Liphis", "Recent");
    QStringList list = settings.value("history").toStringList();
    list.removeAll(path);
    list.prepend(path);
    while (list.size() > 10) list.removeLast();
    settings.setValue("history", list);
    
    // Refresh to show in sidebar immediately
    refresh();
}

QVariantList PlacesModel::entriesByCategory(int category) const
{
    QVariantList out;
    for (const auto &item : m_items) {
        if (item.category != category) continue;
        QVariantMap m;
        m["name"] = item.name;
        m["path"] = item.path;
        m["icon"] = item.icon;
        m["category"] = item.category;
        m["totalBytes"] = item.totalBytes;
        m["usedBytes"] = item.usedBytes;
        out.push_back(m);
    }
    return out;
}

void PlacesModel::setupDefaultPlaces()
{
    m_items.clear();

    // 0. PLACES
    m_items.push_back({"Home", QDir::homePath(), "user-home", 0, 0, 0});
    auto addLoc = [&](QStandardPaths::StandardLocation loc, const QString &name, const QString &icon) {
        QString p = QStandardPaths::writableLocation(loc);
        if (QDir(p).exists()) m_items.push_back({name, p, icon, 0, 0, 0});
    };
    addLoc(QStandardPaths::DocumentsLocation, "Documents", "folder-code");
    addLoc(QStandardPaths::DownloadLocation, "Downloads", "download");
    addLoc(QStandardPaths::PicturesLocation, "Pictures", "photo");
    addLoc(QStandardPaths::MoviesLocation, "Videos", "video");
    addLoc(QStandardPaths::MusicLocation, "Music", "music");
    m_items.push_back({"Trash", "trash:///", "trash", 0, 0, 0});

    // 1. BOOKMARKS
    QSettings bSettings("Liphis", "Bookmarks");
    QVariantList bList = bSettings.value("places").toList();
    for (const auto& v : bList) {
        QVariantMap m = v.toMap();
        QString name = m["name"].toString();
        QString icon = "folder-star";
        if (name.contains("Code", Qt::CaseInsensitive) || name.contains("Dev", Qt::CaseInsensitive) || name.contains("Project", Qt::CaseInsensitive)) {
            icon = "folder-code";
        }
        m_items.push_back({name, m["path"].toString(), icon, 1, 0, 0});
    }

    // 2. DEVICES + 4. NETWORK
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
            QString root = storage.rootPath();
            if (root.startsWith("/proc") || root.startsWith("/sys") || root.startsWith("/dev")) continue;
            QString name = storage.displayName();
            if (name.isEmpty() || name == "/") name = "System Root";
            const qlonglong total = storage.bytesTotal();
            const qlonglong free = storage.bytesAvailable();
            const qlonglong used = (total > 0 && free >= 0) ? (total - free) : 0;

            const QString fsType = QString::fromLatin1(storage.fileSystemType()).toLower();
            bool isNetwork = root.startsWith("/run/user/") || root.contains("/gvfs/")
                || fsType.contains("nfs") || fsType.contains("cifs") || fsType.contains("smb")
                || fsType.contains("fuse.sshfs") || fsType.contains("davfs");

            QString icon = isNetwork ? "network" : (root == "/" ? "drive-harddisk-system" : "drive-removable-media");
            int category = isNetwork ? 4 : 2;
            m_items.push_back({name, root, icon, category, total, used});
        }
    }

    // 3. RECENT
    QSettings rSettings("Liphis", "Recent");
    QStringList rList = rSettings.value("history").toStringList();
    for (const auto& path : rList) {
        QFileInfo fi(path);
        QString icon = fi.isDir() ? "folder-heart" : "doc";
        m_items.push_back({fi.fileName(), path, icon, 3, 0, 0});
    }
}
