#pragma once

#include <QSortFilterProxyModel>

class FileFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool showHidden READ showHidden WRITE setShowHidden NOTIFY showHiddenChanged)
    Q_PROPERTY(qlonglong minSize READ minSize WRITE setMinSize NOTIFY minSizeChanged)
    Q_PROPERTY(qlonglong maxSize READ maxSize WRITE setMaxSize NOTIFY maxSizeChanged)
    Q_PROPERTY(qlonglong minDate READ minDate WRITE setMinDate NOTIFY minDateChanged)
    Q_PROPERTY(qlonglong maxDate READ maxDate WRITE setMaxDate NOTIFY maxDateChanged)
    Q_PROPERTY(QString extensionFilter READ extensionFilter WRITE setExtensionFilter NOTIFY extensionFilterChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(QString typeFilter READ typeFilter WRITE setTypeFilter NOTIFY typeFilterChanged)
    Q_PROPERTY(bool exactMatch READ exactMatch WRITE setExactMatch NOTIFY exactMatchChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit FileFilterProxyModel(QObject *parent = nullptr);

    Q_INVOKABLE void setSortBy(const QString &roleName, bool ascending = true);

    bool showHidden() const;
    void setShowHidden(bool show);

    qlonglong minSize() const;
    void setMinSize(qlonglong size);

    qlonglong maxSize() const;
    void setMaxSize(qlonglong size);

    qlonglong minDate() const;
    void setMinDate(qlonglong date);

    qlonglong maxDate() const;
    void setMaxDate(qlonglong date);

    QString extensionFilter() const;
    void setExtensionFilter(const QString &filter);
    QString searchQuery() const;
    void setSearchQuery(const QString &query);
    QString typeFilter() const;
    void setTypeFilter(const QString &type);
    bool exactMatch() const;
    void setExactMatch(bool exact);

signals:
    void showHiddenChanged();
    void minSizeChanged();
    void maxSizeChanged();
    void minDateChanged();
    void maxDateChanged();
    void extensionFilterChanged();
    void searchQueryChanged();
    void typeFilterChanged();
    void exactMatchChanged();
    void countChanged();

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &source_left,
                  const QModelIndex &source_right) const override;

private:
    void beginFilterChange();
    void endFilterChange();

    bool m_showHidden{false};
    qlonglong m_minSize{-1};
    qlonglong m_maxSize{-1};
    qlonglong m_minDate{-1};
    qlonglong m_maxDate{-1};
    QString m_extensionFilter;
    QString m_searchQuery;
    QString m_typeFilter{"all"}; // all | file | folder
    bool m_exactMatch{false};
};
