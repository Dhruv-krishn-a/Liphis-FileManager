#pragma once

#include <QAbstractListModel>
#include <QVariantMap>
#include <vector>
#include <unordered_map>
#include "FileMeta.hpp"

class FileListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString sortField READ sortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending NOTIFY sortAscendingChanged)

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
        FormattedCTimeRole,
        ATimeRole,
        FormattedATimeRole,
        PermissionsRole,
        ModeRole,
        OwnerRole,
        GroupRole,
        ThumbnailRole,
        IconNameRole,
        MimeTypeRole,
        TypeRole
    };

    // Required overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model API
    void clear();
    void setEntries(std::vector<FileMeta> &&entries);
    void insertBatch(std::vector<FileMeta> &&batch);
    void removeItems(const QStringList &paths);
    void updateThumbnail(const QString &filePath,
                         const QString &thumbPath);

    Q_INVOKABLE QVariantMap metadataForPath(const QString &path) const;
    Q_INVOKABLE QStringList availableExtensions() const;
    Q_INVOKABLE void setSortBy(const QString &field, bool ascending = true);

    QString sortField() const { return m_sortField; }
    bool sortAscending() const { return m_sortAscending; }

signals:
    void countChanged();
    void sortFieldChanged();
    void sortAscendingChanged();

private:
    std::vector<FileMeta> m_entries;
    std::unordered_map<std::string, size_t> m_pathToIndex;
    QString m_sortField = "name";
    bool m_sortAscending = true;
    void applySort();
    void rebuildPathMap();
};
