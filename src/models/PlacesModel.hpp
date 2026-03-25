#pragma once

#include <QAbstractListModel>
#include <vector>
#include <QString>

struct PlaceItem {
    QString name;
    QString path;
    QString icon;
};

class PlacesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        IconRole,
        CategoryRole // 0: Places, 1: Bookmarks, 2: Devices, 3: Recent
    };

    explicit PlacesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void addBookmark(const QString &path, const QString &name);
    Q_INVOKABLE void removeBookmark(int index);
    Q_INVOKABLE void removeBookmarkByPath(const QString &path);
    Q_INVOKABLE bool isBookmarked(const QString &path) const;
    Q_INVOKABLE void addRecent(const QString &path);

private:
    void setupDefaultPlaces();
    struct PlaceItem {
        QString name;
        QString path;
        QString icon;
        int category;
    };
    std::vector<PlaceItem> m_items;
};
