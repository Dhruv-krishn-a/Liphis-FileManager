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

public:
    explicit FileFilterProxyModel(QObject *parent = nullptr);

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

signals:
    void showHiddenChanged();
    void minSizeChanged();
    void maxSizeChanged();
    void minDateChanged();
    void maxDateChanged();
    void extensionFilterChanged();

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &source_left,
                  const QModelIndex &source_right) const override;

private:
    bool m_showHidden{false};
    qlonglong m_minSize{-1};
    qlonglong m_maxSize{-1};
    qlonglong m_minDate{-1};
    qlonglong m_maxDate{-1};
    QString m_extensionFilter;
};
