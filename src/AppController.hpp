#pragma once

#include <QObject>
#include <QString>
#include <atomic>
#include <cstdint>
#include <QVariantMap>
#include <QHash>
#include <QSet>
#include <QElapsedTimer>
#include <QTimer>

#include "FileListModel.hpp"
#include "FileTreeModel.hpp"
#include "FileFilterProxyModel.hpp"
#include "PlacesModel.hpp"
#include "ThreadPool.hpp"
#include "ThumbnailManager.hpp"
#include "FileCommand.hpp"
#include <QFileSystemWatcher>

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* fileModel READ fileModel CONSTANT)
    Q_PROPERTY(QObject* treeModel READ treeModel CONSTANT)
    Q_PROPERTY(QObject* placesModel READ placesModel NOTIFY placesModelChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString selectedPath READ selectedPath NOTIFY selectedPathChanged)
    Q_PROPERTY(QStringList selectedPaths READ selectedPaths NOTIFY selectedPathsChanged)
    Q_PROPERTY(int selectionRevision READ selectionRevision NOTIFY selectedPathsChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectedPathsChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY canGoBackChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY canGoForwardChanged)
    Q_PROPERTY(QString homePath READ homePath CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    
    // UI State
    Q_PROPERTY(bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles NOTIFY showHiddenFilesChanged)
    Q_PROPERTY(bool hasClipboard READ hasClipboard NOTIFY clipboardChanged)
    Q_PROPERTY(QStringList clipboardPaths READ clipboardPaths NOTIFY clipboardChanged)
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
    Q_PROPERTY(bool searchContent READ searchContent WRITE setSearchContent NOTIFY searchContentChanged)
    Q_PROPERTY(QStringList recentSearches READ recentSearches NOTIFY recentSearchesChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(QString undoDescription READ undoDescription NOTIFY canUndoChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    QObject* fileModel();
    QObject* treeModel();
    QObject* placesModel();
    QString currentPath() const;
    QString homePath() const;
    QString title() const;
    bool loading() const;
    bool canGoBack() const;
    bool canGoForward() const;
    
    bool canUndo() const;
    bool canRedo() const;
    QString undoDescription() const;
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    
    bool showHiddenFiles() const;
    void setShowHiddenFiles(bool show);
    bool hasClipboard() const;
    QStringList clipboardPaths() const;
    bool isCutOp() const;

    int iconSize() const;
    void setIconSize(int size);

    Q_INVOKABLE QString viewMode() const;
    Q_INVOKABLE void setViewMode(const QString &mode);

    QStringList availableExtensions() const;
    QVariantMap gitStatus() const;
    int bookmarksRevision() const { return m_bookmarksRevision; }
    QString searchMode() const { return m_searchMode; }
    void setSearchMode(const QString &mode);
    QString searchScope() const { return m_searchScope; }
    void setSearchScope(const QString &scope);
    bool searchInProgress() const { return m_searchInProgress; }
    QString activeSearchTerm() const { return m_activeSearchTerm; }
    bool searchContent() const { return m_searchContent; }
    void setSearchContent(bool enabled);
    QStringList recentSearches() const { return m_recentSearches; }

    // Navigation
    Q_INVOKABLE void openPath(const QString &path);
    Q_INVOKABLE void openWith(const QString &path);
    Q_INVOKABLE void openWithApp(const QString &path, const QString &appCommand);
    Q_INVOKABLE QVariantList getAssociatedApps(const QString &path);
    Q_INVOKABLE QVariantList getAllApplications();
    Q_INVOKABLE void setDefaultApp(const QString &mimeType, const QString &desktopFile);
    Q_INVOKABLE QString getMimeType(const QString &path);
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
    Q_INVOKABLE void createFile(const QString &name);
    Q_INVOKABLE void renameItem(const QString &oldPath, const QString &newName);
    Q_INVOKABLE void bulkRename(const QStringList &paths, const QString &prefix, const QString &suffix, const QString &find, const QString &replace);
    Q_INVOKABLE void deleteItem(const QString &path);
    Q_INVOKABLE void deleteItems(const QStringList &paths);
    Q_INVOKABLE void copyItem(const QString &path);
    Q_INVOKABLE void cutItem(const QString &path);
    Q_INVOKABLE void pasteItem();
    Q_INVOKABLE void clearClipboard();

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
    int selectionRevision() const { return m_selectionRevision; }
    bool hasSelection() const;
    
    // Security Helper
    QString safePath(const QString &path) const;

    Q_INVOKABLE QVariantMap metadataForPath(const QString &path) const;
    Q_INVOKABLE void requestThumbnailsForRange(int firstIndex, int lastIndex);
    Q_INVOKABLE QString getFilePreview(const QString &path);
    Q_INVOKABLE void openInTerminal(const QString &path);
    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE void trashItems(const QStringList &paths);
    Q_INVOKABLE void restoreFromTrash(const QStringList &paths);
    Q_INVOKABLE void emptyTrash();
    Q_INVOKABLE void analyseFolder(const QString &path);
    Q_INVOKABLE QString runGitCommand(const QString &cmd, const QString &path);
    Q_INVOKABLE QVariantMap getFolderMetadata(const QString &path);
    Q_INVOKABLE void requestFolderSize(const QString &path);
    
    // New Advanced Features
    Q_INVOKABLE void compressItems(const QStringList &paths);
    Q_INVOKABLE void extractItem(const QString &path);
    Q_INVOKABLE void connectRemote(const QString &url);
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
    Q_INVOKABLE void dropItems(const QStringList &paths, const QString &targetDir, bool isCopy);

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
    void searchContentChanged();
    void activeSearchTermChanged();
    void recentSearchesChanged();
    void requestSearchClear();
    void openWithRequested(const QString &path);
    void canUndoChanged();
    void canRedoChanged();
    void folderSizeResolved(const QString &path, qlonglong size);
    
    void operationError(const QString &message);
    void operationSuccess(const QString &message);
    void operationProgress(float progress);

private:
    void loadPathInternal(const QString &path); 
    void reloadCurrentDirectoryModel(bool preserveSelection);
    void removeFromSelection(const QStringList &paths);
    void addEntriesForPaths(const QStringList &paths, bool selectAdded = false);
    bool isTrashPath(const QString &path) const;
    void loadTrashInternal();

    FileListModel m_fileModel;
    FileTreeModel m_treeModel;
    FileFilterProxyModel m_proxyModel;
    PlacesModel* m_placesModel = nullptr;
    FileCommandHistory m_historyStack;

    QString m_currentPath;
    std::atomic<bool> m_cancelRequested{false};
    std::atomic<std::uint64_t> m_operationGeneration{0};
    bool m_loading{false};

    ThumbnailManager* m_thumbnailManager = nullptr;
    QFileSystemWatcher m_watcher;
    QTimer* m_dirChangeTimer = nullptr;
    QElapsedTimer m_lastUserInitiatedOp;

    QString m_selectedPath;
    QStringList m_selectedPaths;
    QVector<QString> m_history;
    int m_historyIndex = -1;
    
    // UI State
    int m_iconSize = 64;
    QString m_viewMode = "grid";
    
    // Clipboard
    QStringList m_clipboardPaths;
    bool m_isCutOp{false};

    QVariantMap m_gitStatus;
    int m_bookmarksRevision{0};
    QString m_searchMode{"local"};
    QString m_searchScope{"current"}; // current | home | mounted
    bool m_searchContent{false};
    bool m_searchInProgress{false};
    int m_selectionRevision{0};
    QString m_activeSearchTerm;
    QStringList m_recentSearches;
    bool m_globalSearchActive{false};
    void updateGitStatus();
    QTimer* m_gitStatusTimer = nullptr;
    QTimer* m_searchTimer = nullptr;
    QHash<QString, qlonglong> m_folderSizeCache;
    QSet<QString> m_pendingFolderSizeRequests;
    void loadRecentSearches();
    void persistRecentSearches();
public:
    Q_INVOKABLE void setThumbnailManager(ThumbnailManager* manager);
    Q_INVOKABLE void setPlacesModel(PlacesModel* model);
};
