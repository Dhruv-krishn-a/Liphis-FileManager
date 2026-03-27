#pragma once

#include <QAbstractListModel>
#include <QVariantMap>
#include <vector>
#include "FileMeta.hpp"

class FileListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit FileListModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        SizeRole,
        FormattedSizeRole,
        IsDirRole,
        MTimeRole,
        FormattedDateRole,
        CTimeRole,
        ATimeRole,
        PermissionsRole,
        ModeRole,
        OwnerRole,
        GroupRole,
        ThumbnailRole,
        IconNameRole
    };

    // Required overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model API
    void clear();
    void setEntries(std::vector<FileMeta> &&entries);
    void insertBatch(std::vector<FileMeta> &&batch);
    void updateThumbnail(const QString &filePath,
                         const QString &thumbPath);

    Q_INVOKABLE QVariantMap metadataForPath(const QString &path) const;
    Q_INVOKABLE QStringList availableExtensions() const;

signals:
    void countChanged();

private:
    std::vector<FileMeta> m_entries;
};
