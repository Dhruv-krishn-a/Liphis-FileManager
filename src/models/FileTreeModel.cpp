#include "FileTreeModel.hpp"

FileTreeModel::FileTreeModel(QObject *parent)
    : QFileSystemModel(parent)
{
    setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    setRootPath("");
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case NameRole:
        return QFileSystemModel::data(index, QFileSystemModel::FileNameRole);
    case PathRole:
        return QFileSystemModel::data(index, QFileSystemModel::FilePathRole);
    case IsDirRole:
        return isDir(index);
    case IconNameRole:
        return isDir(index) ? "folder" : "text-x-generic";
    default:
        return QFileSystemModel::data(index, role);
    }
}

QHash<int, QByteArray> FileTreeModel::roleNames() const
{
    QHash<int, QByteArray> roles = QFileSystemModel::roleNames();
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    roles[IsDirRole] = "isDir";
    roles[IconNameRole] = "iconName";
    return roles;
}
