#pragma once

#include <QObject>
#include <QString>
#include <atomic>
#include <cstdint>
#include <QVariantMap>

#include "FileListModel.hpp"
#include "FileFilterProxyModel.hpp"
#include "PlacesModel.hpp"
#include "ThreadPool.hpp"
#include "ThumbnailManager.hpp"
#include <QFileSystemWatcher>

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* fileModel READ fileModel CONSTANT)
    Q_PROPERTY(QObject* placesModel READ placesModel NOTIFY placesModelChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString selectedPath READ selectedPath NOTIFY selectedPathChanged)
    Q_PROPERTY(QStringList selectedPaths READ selectedPaths NOTIFY selectedPathsChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectedPathsChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY canGoBackChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY canGoForwardChanged)
    Q_PROPERTY(QString homePath READ homePath CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    
    // UI State
    Q_PROPERTY(bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles NOTIFY showHiddenFilesChanged)
    Q_PROPERTY(bool hasClipboard READ hasClipboard NOTIFY clipboardChanged)
    Q_PROPERTY(QString clipboardPath READ clipboardPath NOTIFY clipboardChanged)
    Q_PROPERTY(bool isCutOp READ isCutOp NOTIFY clipboardChanged)
    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(QString viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)
    Q_PROPERTY(QStringList availableExtensions READ availableExtensions NOTIFY availableExtensionsChanged)
    Q_PROPERTY(QVariantMap gitStatus READ gitStatus NOTIFY gitStatusChanged)
    Q_PROPERTY(int bookmarksRevision READ bookmarksRevision NOTIFY bookmarksRevisionChanged)
    Q_PROPERTY(QString searchMode READ searchMode WRITE setSearchMode NOTIFY searchModeChanged)
    Q_PROPERTY(QString searchScope READ searchScope WRITE setSearchScope NOTIFY searchScopeChanged)
    Q_PROPERTY(bool searchInProgress READ searchInProgress NOTIFY searchInProgressChanged)
    Q_PROPERTY(QString activeSearchTerm READ activeSearchTerm NOTIFY activeSearchTermChanged)
    Q_PROPERTY(QStringList recentSearches READ recentSearches NOTIFY recentSearchesChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    QObject* fileModel();
    QObject* placesModel();
    QString currentPath() const;
    QString homePath() const;
    QString title() const;
    bool loading() const;
    bool canGoBack() const;
    bool canGoForward() const;
    
    bool showHiddenFiles() const;
    void setShowHiddenFiles(bool show);
    bool hasClipboard() const;
    QString clipboardPath() const;
    bool isCutOp() const;

    int iconSize() const;
    void setIconSize(int size);

    QString viewMode() const;
    void setViewMode(const QString &mode);

    QStringList availableExtensions() const;
    QVariantMap gitStatus() const;
    int bookmarksRevision() const { return m_bookmarksRevision; }
    QString searchMode() const { return m_searchMode; }
    void setSearchMode(const QString &mode);
    QString searchScope() const { return m_searchScope; }
    void setSearchScope(const QString &scope);
    bool searchInProgress() const { return m_searchInProgress; }
    QString activeSearchTerm() const { return m_activeSearchTerm; }
    QStringList recentSearches() const { return m_recentSearches; }

    // Navigation
    Q_INVOKABLE void openPath(const QString &path);
    Q_INVOKABLE void goUp();
    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goForward();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setSearchText(const QString &text);
    Q_INVOKABLE void startGlobalSearch(const QString &pattern);
    Q_INVOKABLE void applySearchQuery(const QString &query);
    Q_INVOKABLE void saveSearchQuery(const QString &query);
    Q_INVOKABLE void cancelSearch();

    // File Operations
    Q_INVOKABLE void createFolder(const QString &name);
    Q_INVOKABLE void renameItem(const QString &oldPath, const QString &newName);
    Q_INVOKABLE void bulkRename(const QStringList &paths, const QString &prefix, const QString &suffix, const QString &find, const QString &replace);
    Q_INVOKABLE void deleteItem(const QString &path);
    Q_INVOKABLE void copyItem(const QString &path);
    Q_INVOKABLE void cutItem(const QString &path);
    Q_INVOKABLE void pasteItem();

    // Utility
    Q_INVOKABLE void requestThumbnail(const QString &path);
    Q_INVOKABLE void selectPath(const QString &path);
    Q_INVOKABLE void toggleSelection(const QString &path);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void selectRangeByIndexes(int from, int to);
    Q_INVOKABLE QString pathAtIndex(int index) const;
    Q_INVOKABLE QString nameAtIndex(int index) const;
    Q_INVOKABLE int indexOfPath(const QString &path) const;
    Q_INVOKABLE void startRename(const QString &path);
    QString selectedPath() const;
    QStringList selectedPaths() const;
    bool hasSelection() const;

    Q_INVOKABLE QVariantMap metadataForPath(const QString &path) const;
    Q_INVOKABLE void requestThumbnailsForRange(int firstIndex, int lastIndex);
    Q_INVOKABLE QString getFilePreview(const QString &path);
    Q_INVOKABLE void openInTerminal(const QString &path);
    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE void trashItems(const QStringList &paths);
    Q_INVOKABLE void analyseFolder(const QString &path);
    
    // New Advanced Features
    Q_INVOKABLE void compressItems(const QStringList &paths);
    Q_INVOKABLE void extractItem(const QString &path);
    Q_INVOKABLE void openInCode(const QString &path);
    Q_INVOKABLE void addToBookmarks(const QString &path, const QString &name);
    Q_INVOKABLE void removeFromBookmarks(int index);
    Q_INVOKABLE void removeBookmarkByPath(const QString &path);
    Q_INVOKABLE bool isBookmarked(const QString &path) const;
    Q_INVOKABLE void toggleBookmark(const QString &path, const QString &name);
    Q_INVOKABLE void openAsRoot(const QString &path);
    Q_INVOKABLE void openInNewWindow(const QString &path);
    Q_INVOKABLE QString computeChecksum(const QString &path);
    Q_INVOKABLE void duplicateItem(const QString &path);
    Q_INVOKABLE void createSymlink(const QString &target, const QString &linkName);
    Q_INVOKABLE void makeExecutable(const QString &path);
    Q_INVOKABLE void setPermissions(const QString &path, const QString &mode);
    Q_INVOKABLE void setWallpaper(const QString &path);
    Q_INVOKABLE void mountRemote(const QString &url);

signals:
    void currentPathChanged();
    void loadingChanged();
    void selectedPathChanged();
    void selectedPathsChanged();
    void canGoBackChanged();
    void canGoForwardChanged();
    void showHiddenFilesChanged();
    void clipboardChanged();
    void titleChanged();
    void iconSizeChanged();
    void viewModeChanged();
    void renameRequested(const QString &path);
    void availableExtensionsChanged();
    void gitStatusChanged();
    void analyseRequested(const QString &path);
    void placesModelChanged();
    void bookmarksRevisionChanged();
    void searchModeChanged();
    void searchScopeChanged();
    void searchInProgressChanged();
    void activeSearchTermChanged();
    void recentSearchesChanged();
    void requestSearchClear();
    
    void operationError(const QString &message);
    void operationSuccess(const QString &message);
    void operationProgress(float progress);

private:
    void loadPathInternal(const QString &path); 

    FileListModel m_fileModel;
    FileFilterProxyModel m_proxyModel;
    PlacesModel* m_placesModel = nullptr;

    QString m_currentPath;
    std::atomic<bool> m_cancelRequested{false};
    std::atomic<std::uint64_t> m_operationGeneration{0};
    bool m_loading{false};

    ThumbnailManager* m_thumbnailManager = nullptr;
    QFileSystemWatcher m_watcher;

    QString m_selectedPath;
    QStringList m_selectedPaths;
    QVector<QString> m_history;
    int m_historyIndex = -1;
    
    // UI State
    int m_iconSize = 64;
    QString m_viewMode = "grid";
    
    // Clipboard
    QString m_clipboardPath;
    bool m_isCutOp{false};

    QVariantMap m_gitStatus;
    int m_bookmarksRevision{0};
    QString m_searchMode{"local"};
    QString m_searchScope{"current"}; // current | home | mounted
    bool m_searchInProgress{false};
    QString m_activeSearchTerm;
    QStringList m_recentSearches;
    bool m_globalSearchActive{false};
    void updateGitStatus();
    void loadRecentSearches();
    void persistRecentSearches();
public:
    Q_INVOKABLE void setThumbnailManager(ThumbnailManager* manager);
    Q_INVOKABLE void setPlacesModel(PlacesModel* model);
};
