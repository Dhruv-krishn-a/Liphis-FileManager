#include "PlacesModel.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QStorageInfo>
#include <QSettings>

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
    QSettings settings("Liphis", "Bookmarks");
    QVariantList list = settings.value("places").toList();
    
    QVariantMap item;
    item["name"] = name;
    item["path"] = path;
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
    QString path = m_items[index].path;
    for (int i=0; i<list.size(); ++i) {
        if (list[i].toMap()["path"].toString() == path) {
            list.removeAt(i);
            break;
        }
    }
    
    settings.setValue("places", list);
    refresh();
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

void PlacesModel::setupDefaultPlaces()
{
    m_items.clear();

    // 0. PLACES
    m_items.push_back({"Home", QDir::homePath(), "user-home", 0});
    auto addLoc = [&](QStandardPaths::StandardLocation loc, const QString &name, const QString &icon) {
        QString p = QStandardPaths::writableLocation(loc);
        if (QDir(p).exists()) m_items.push_back({name, p, icon, 0});
    };
    addLoc(QStandardPaths::DocumentsLocation, "Documents", "folder-documents");
    addLoc(QStandardPaths::DownloadLocation, "Downloads", "folder-download");
    addLoc(QStandardPaths::PicturesLocation, "Pictures", "folder-pictures");

    // 1. BOOKMARKS
    QSettings bSettings("Liphis", "Bookmarks");
    QVariantList bList = bSettings.value("places").toList();
    for (const auto& v : bList) {
        QVariantMap m = v.toMap();
        m_items.push_back({m["name"].toString(), m["path"].toString(), "user-bookmarks", 1});
    }

    // 2. DEVICES
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
            QString root = storage.rootPath();
            if (root.startsWith("/proc") || root.startsWith("/sys") || root.startsWith("/dev") || root.startsWith("/run/user")) continue;
            QString name = storage.displayName();
            if (name.isEmpty() || name == "/") name = "System Root";
            QString icon = root == "/" ? "drive-harddisk-system" : "drive-removable-media";
            m_items.push_back({name, root, icon, 2});
        }
    }

    // 3. RECENT
    QSettings rSettings("Liphis", "Recent");
    QStringList rList = rSettings.value("history").toStringList();
    for (const auto& path : rList) {
        m_items.push_back({QFileInfo(path).fileName(), path, "document-open-recent", 3});
    }
}
