#include "FileFilterProxyModel.hpp"
#include "FileListModel.hpp"
#include <QFileInfo>

FileFilterProxyModel::FileFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setSortRole(FileListModel::NameRole);
    setDynamicSortFilter(true);
    sort(0); 

    connect(this, &QAbstractItemModel::modelReset, this, &FileFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsInserted, this, &FileFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &FileFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::layoutChanged, this, &FileFilterProxyModel::countChanged);
}

void FileFilterProxyModel::setSortBy(const QString &roleName, bool ascending)
{
    if (roleName == "name") setSortRole(FileListModel::NameRole);
    else if (roleName == "date") setSortRole(FileListModel::MTimeRole);
    else if (roleName == "size") setSortRole(FileListModel::SizeRole);
    
    sort(0, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

bool FileFilterProxyModel::showHidden() const { return m_showHidden; }
void FileFilterProxyModel::setShowHidden(bool show) {
    if (m_showHidden == show) return;
    m_showHidden = show;
    emit showHiddenChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

qlonglong FileFilterProxyModel::minSize() const { return m_minSize; }
void FileFilterProxyModel::setMinSize(qlonglong size) {
    if (m_minSize == size) return;
    m_minSize = size;
    emit minSizeChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

qlonglong FileFilterProxyModel::maxSize() const { return m_maxSize; }
void FileFilterProxyModel::setMaxSize(qlonglong size) {
    if (m_maxSize == size) return;
    m_maxSize = size;
    emit maxSizeChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

qlonglong FileFilterProxyModel::minDate() const { return m_minDate; }
void FileFilterProxyModel::setMinDate(qlonglong date) {
    if (m_minDate == date) return;
    m_minDate = date;
    emit minDateChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

qlonglong FileFilterProxyModel::maxDate() const { return m_maxDate; }
void FileFilterProxyModel::setMaxDate(qlonglong date) {
    if (m_maxDate == date) return;
    m_maxDate = date;
    emit maxDateChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

QString FileFilterProxyModel::extensionFilter() const { return m_extensionFilter; }
void FileFilterProxyModel::setExtensionFilter(const QString &filter) {
    if (m_extensionFilter == filter) return;
    m_extensionFilter = filter;
    emit extensionFilterChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

QString FileFilterProxyModel::searchQuery() const { return m_searchQuery; }
void FileFilterProxyModel::setSearchQuery(const QString &query) {
    if (m_searchQuery == query) return;
    m_searchQuery = query;
    emit searchQueryChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    sort(0);
    emit countChanged();
}

QString FileFilterProxyModel::typeFilter() const { return m_typeFilter; }
void FileFilterProxyModel::setTypeFilter(const QString &type) {
    QString normalized = type.trimmed().toLower();
    if (normalized != "file" && normalized != "folder") normalized = "all";
    if (m_typeFilter == normalized) return;
    m_typeFilter = normalized;
    emit typeFilterChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    emit countChanged();
}

bool FileFilterProxyModel::exactMatch() const { return m_exactMatch; }
void FileFilterProxyModel::setExactMatch(bool exact) {
    if (m_exactMatch == exact) return;
    m_exactMatch = exact;
    emit exactMatchChanged();
    QSortFilterProxyModel::beginFilterChange();
    QSortFilterProxyModel::endFilterChange();
    sort(0);
    emit countChanged();
}

bool FileFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    const QString name = sourceModel()->data(index, FileListModel::NameRole).toString();
    const bool isDir = sourceModel()->data(index, FileListModel::IsDirRole).toBool();
    
    // Hidden file check
    if (!m_showHidden) {
        if (name.startsWith(".")) return false;
    }

    if (m_typeFilter == "file" && isDir) return false;
    if (m_typeFilter == "folder" && !isDir) return false;

    // Size filter
    if (m_minSize >= 0 || m_maxSize >= 0) {
        if (!isDir) {
            qlonglong size = sourceModel()->data(index, FileListModel::SizeRole).toLongLong();
            if (m_minSize >= 0 && size < m_minSize) return false;
            if (m_maxSize >= 0 && size > m_maxSize) return false;
        }
    }

    // Date filter
    if (m_minDate >= 0 || m_maxDate >= 0) {
        qlonglong mtime = sourceModel()->data(index, FileListModel::MTimeRole).toLongLong();
        if (m_minDate >= 0 && mtime < m_minDate) return false;
        if (m_maxDate >= 0 && mtime > m_maxDate) return false;
    }

    // Extension filter
    if (!m_extensionFilter.isEmpty()) {
        if (isDir) return false; // Hide folders if we are filtering by extension
        
        QString lowerName = name.toLower();
        if (m_extensionFilter == "no extension") {
            if (lowerName.contains(".")) return false;
        } else {
            if (!lowerName.endsWith("." + m_extensionFilter.toLower())) return false;
        }
    }

    if (!m_searchQuery.trimmed().isEmpty()) {
        const QString q = m_searchQuery.trimmed().toLower();
        const QString lowerName = name.toLower();
        if (m_exactMatch) {
            if (lowerName != q) return false;
        } else if (!lowerName.contains(q)) {
            return false;
        }
    }

    return true;
}

bool FileFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    bool leftIsDir = sourceModel()->data(source_left, FileListModel::IsDirRole).toBool();
    bool rightIsDir = sourceModel()->data(source_right, FileListModel::IsDirRole).toBool();

    if (leftIsDir != rightIsDir) {
        bool isLess = leftIsDir && !rightIsDir;
        return sortOrder() == Qt::AscendingOrder ? isLess : !isLess;
    }

    const QString q = m_searchQuery.trimmed().toLower();
    if (!q.isEmpty()) {
        const QString leftName = sourceModel()->data(source_left, FileListModel::NameRole).toString().toLower();
        const QString rightName = sourceModel()->data(source_right, FileListModel::NameRole).toString().toLower();

        auto rank = [&](const QString &name) {
            if (m_exactMatch) return name == q ? 0 : 3;
            if (name.startsWith(q)) return 0;
            if (name == q) return 1;
            if (name.contains(q)) return 2;
            return 3;
        };
        const int lr = rank(leftName);
        const int rr = rank(rightName);
        if (lr != rr) return lr < rr;
    }

    bool result = QSortFilterProxyModel::lessThan(source_left, source_right);
    if (!result && !QSortFilterProxyModel::lessThan(source_right, source_left)) {
        // Values are equal, fallback to name
        const QString leftName = sourceModel()->data(source_left, FileListModel::NameRole).toString().toLower();
        const QString rightName = sourceModel()->data(source_right, FileListModel::NameRole).toString().toLower();
        return leftName < rightName;
    }
    return result;
}
