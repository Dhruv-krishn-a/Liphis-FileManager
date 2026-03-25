#include "FileFilterProxyModel.hpp"
#include "FileListModel.hpp"

FileFilterProxyModel::FileFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setSortRole(FileListModel::NameRole);
    setDynamicSortFilter(true);
    sort(0); 
}

bool FileFilterProxyModel::showHidden() const { return m_showHidden; }
void FileFilterProxyModel::setShowHidden(bool show) {
    if (m_showHidden == show) return;
    m_showHidden = show;
    emit showHiddenChanged();
    beginFilterChange();
    endFilterChange();
}

qlonglong FileFilterProxyModel::minSize() const { return m_minSize; }
void FileFilterProxyModel::setMinSize(qlonglong size) {
    if (m_minSize == size) return;
    m_minSize = size;
    emit minSizeChanged();
    beginFilterChange();
    endFilterChange();
}

qlonglong FileFilterProxyModel::maxSize() const { return m_maxSize; }
void FileFilterProxyModel::setMaxSize(qlonglong size) {
    if (m_maxSize == size) return;
    m_maxSize = size;
    emit maxSizeChanged();
    beginFilterChange();
    endFilterChange();
}

qlonglong FileFilterProxyModel::minDate() const { return m_minDate; }
void FileFilterProxyModel::setMinDate(qlonglong date) {
    if (m_minDate == date) return;
    m_minDate = date;
    emit minDateChanged();
    beginFilterChange();
    endFilterChange();
}

qlonglong FileFilterProxyModel::maxDate() const { return m_maxDate; }
void FileFilterProxyModel::setMaxDate(qlonglong date) {
    if (m_maxDate == date) return;
    m_maxDate = date;
    emit maxDateChanged();
    beginFilterChange();
    endFilterChange();
}

QString FileFilterProxyModel::extensionFilter() const { return m_extensionFilter; }
void FileFilterProxyModel::setExtensionFilter(const QString &filter) {
    if (m_extensionFilter == filter) return;
    m_extensionFilter = filter;
    emit extensionFilterChanged();
    beginFilterChange();
    endFilterChange();
}

bool FileFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    
    // Hidden file check
    if (!m_showHidden) {
        QString name = sourceModel()->data(index, FileListModel::NameRole).toString();
        if (name.startsWith(".")) return false;
    }

    // Size filter
    if (m_minSize >= 0 || m_maxSize >= 0) {
        bool isDir = sourceModel()->data(index, FileListModel::IsDirRole).toBool();
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
        bool isDir = sourceModel()->data(index, FileListModel::IsDirRole).toBool();
        if (isDir) return false; // Hide folders if we are filtering by extension
        
        QString name = sourceModel()->data(index, FileListModel::NameRole).toString().toLower();
        if (m_extensionFilter == "no extension") {
            if (name.contains(".")) return false;
        } else {
            if (!name.endsWith("." + m_extensionFilter.toLower())) return false;
        }
    }

    // Regex filter
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool FileFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    bool leftIsDir = sourceModel()->data(source_left, FileListModel::IsDirRole).toBool();
    bool rightIsDir = sourceModel()->data(source_right, FileListModel::IsDirRole).toBool();

    if (leftIsDir != rightIsDir) {
        return leftIsDir && !rightIsDir;
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
