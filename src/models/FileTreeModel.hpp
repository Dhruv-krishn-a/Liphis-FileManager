#pragma once

#include <QFileSystemModel>
#include <QObject>

class FileTreeModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FileTreeModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        IsDirRole,
        IconNameRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
