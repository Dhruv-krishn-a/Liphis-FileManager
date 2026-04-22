/****************************************************************************
** Meta object code from reading C++ file 'AppController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/AppController.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppController.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13AppControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto AppController::qt_create_metaobjectdata<qt_meta_tag_ZN13AppControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppController",
        "currentPathChanged",
        "",
        "loadingChanged",
        "selectedPathChanged",
        "selectedPathsChanged",
        "canGoBackChanged",
        "canGoForwardChanged",
        "showHiddenFilesChanged",
        "clipboardChanged",
        "titleChanged",
        "iconSizeChanged",
        "viewModeChanged",
        "renameRequested",
        "path",
        "availableExtensionsChanged",
        "gitStatusChanged",
        "analyseRequested",
        "placesModelChanged",
        "bookmarksRevisionChanged",
        "searchModeChanged",
        "searchScopeChanged",
        "searchInProgressChanged",
        "searchContentChanged",
        "activeSearchTermChanged",
        "recentSearchesChanged",
        "requestSearchClear",
        "openWithRequested",
        "canUndoChanged",
        "canRedoChanged",
        "folderSizeResolved",
        "size",
        "operationError",
        "message",
        "operationSuccess",
        "operationProgress",
        "progress",
        "undo",
        "redo",
        "viewMode",
        "setViewMode",
        "mode",
        "openPath",
        "openWith",
        "openWithApp",
        "appCommand",
        "getAssociatedApps",
        "QVariantList",
        "getAllApplications",
        "setDefaultApp",
        "mimeType",
        "desktopFile",
        "getMimeType",
        "goUp",
        "goBack",
        "goForward",
        "refresh",
        "setSearchText",
        "text",
        "startGlobalSearch",
        "pattern",
        "applySearchQuery",
        "query",
        "saveSearchQuery",
        "cancelSearch",
        "createFolder",
        "name",
        "createFile",
        "renameItem",
        "oldPath",
        "newName",
        "bulkRename",
        "paths",
        "prefix",
        "suffix",
        "find",
        "replace",
        "deleteItem",
        "deleteItems",
        "copyItem",
        "cutItem",
        "pasteItem",
        "clearClipboard",
        "requestThumbnail",
        "selectPath",
        "toggleSelection",
        "clearSelection",
        "selectAll",
        "selectRangeByIndexes",
        "from",
        "to",
        "pathAtIndex",
        "index",
        "nameAtIndex",
        "indexOfPath",
        "startRename",
        "metadataForPath",
        "QVariantMap",
        "requestThumbnailsForRange",
        "firstIndex",
        "lastIndex",
        "getFilePreview",
        "openInTerminal",
        "copyToClipboard",
        "trashItems",
        "restoreFromTrash",
        "emptyTrash",
        "analyseFolder",
        "runGitCommand",
        "cmd",
        "getFolderMetadata",
        "requestFolderSize",
        "compressItems",
        "extractItem",
        "connectRemote",
        "url",
        "openInCode",
        "addToBookmarks",
        "removeFromBookmarks",
        "removeBookmarkByPath",
        "isBookmarked",
        "toggleBookmark",
        "openAsRoot",
        "openInNewWindow",
        "computeChecksum",
        "duplicateItem",
        "createSymlink",
        "target",
        "linkName",
        "makeExecutable",
        "setPermissions",
        "setWallpaper",
        "mountRemote",
        "dropItems",
        "targetDir",
        "isCopy",
        "setThumbnailManager",
        "ThumbnailManager*",
        "manager",
        "setPlacesModel",
        "PlacesModel*",
        "model",
        "fileModel",
        "treeModel",
        "placesModel",
        "currentPath",
        "loading",
        "selectedPath",
        "selectedPaths",
        "selectionRevision",
        "hasSelection",
        "canGoBack",
        "canGoForward",
        "homePath",
        "title",
        "showHiddenFiles",
        "hasClipboard",
        "clipboardPaths",
        "isCutOp",
        "iconSize",
        "availableExtensions",
        "gitStatus",
        "bookmarksRevision",
        "searchMode",
        "searchScope",
        "searchInProgress",
        "activeSearchTerm",
        "searchContent",
        "recentSearches",
        "canUndo",
        "canRedo",
        "undoDescription"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'currentPathChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loadingChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedPathChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedPathsChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'canGoBackChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'canGoForwardChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'showHiddenFilesChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clipboardChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'titleChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'iconSizeChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'viewModeChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'renameRequested'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'availableExtensionsChanged'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gitStatusChanged'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'analyseRequested'
        QtMocHelpers::SignalData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'placesModelChanged'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bookmarksRevisionChanged'
        QtMocHelpers::SignalData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchModeChanged'
        QtMocHelpers::SignalData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchScopeChanged'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchInProgressChanged'
        QtMocHelpers::SignalData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchContentChanged'
        QtMocHelpers::SignalData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'activeSearchTermChanged'
        QtMocHelpers::SignalData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'recentSearchesChanged'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestSearchClear'
        QtMocHelpers::SignalData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'openWithRequested'
        QtMocHelpers::SignalData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'canUndoChanged'
        QtMocHelpers::SignalData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'canRedoChanged'
        QtMocHelpers::SignalData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'folderSizeResolved'
        QtMocHelpers::SignalData<void(const QString &, qlonglong)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::LongLong, 31 },
        }}),
        // Signal 'operationError'
        QtMocHelpers::SignalData<void(const QString &)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 33 },
        }}),
        // Signal 'operationSuccess'
        QtMocHelpers::SignalData<void(const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 33 },
        }}),
        // Signal 'operationProgress'
        QtMocHelpers::SignalData<void(float)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 36 },
        }}),
        // Method 'undo'
        QtMocHelpers::MethodData<void()>(37, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'redo'
        QtMocHelpers::MethodData<void()>(38, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'viewMode'
        QtMocHelpers::MethodData<QString() const>(39, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'setViewMode'
        QtMocHelpers::MethodData<void(const QString &)>(40, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 41 },
        }}),
        // Method 'openPath'
        QtMocHelpers::MethodData<void(const QString &)>(42, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'openWith'
        QtMocHelpers::MethodData<void(const QString &)>(43, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'openWithApp'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(44, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 45 },
        }}),
        // Method 'getAssociatedApps'
        QtMocHelpers::MethodData<QVariantList(const QString &)>(46, 2, QMC::AccessPublic, 0x80000000 | 47, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'getAllApplications'
        QtMocHelpers::MethodData<QVariantList()>(48, 2, QMC::AccessPublic, 0x80000000 | 47),
        // Method 'setDefaultApp'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(49, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 50 }, { QMetaType::QString, 51 },
        }}),
        // Method 'getMimeType'
        QtMocHelpers::MethodData<QString(const QString &)>(52, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'goUp'
        QtMocHelpers::MethodData<void()>(53, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'goBack'
        QtMocHelpers::MethodData<void()>(54, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'goForward'
        QtMocHelpers::MethodData<void()>(55, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'refresh'
        QtMocHelpers::MethodData<void()>(56, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'setSearchText'
        QtMocHelpers::MethodData<void(const QString &)>(57, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 58 },
        }}),
        // Method 'startGlobalSearch'
        QtMocHelpers::MethodData<void(const QString &)>(59, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 60 },
        }}),
        // Method 'applySearchQuery'
        QtMocHelpers::MethodData<void(const QString &)>(61, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 62 },
        }}),
        // Method 'saveSearchQuery'
        QtMocHelpers::MethodData<void(const QString &)>(63, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 62 },
        }}),
        // Method 'cancelSearch'
        QtMocHelpers::MethodData<void()>(64, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'createFolder'
        QtMocHelpers::MethodData<void(const QString &)>(65, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 66 },
        }}),
        // Method 'createFile'
        QtMocHelpers::MethodData<void(const QString &)>(67, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 66 },
        }}),
        // Method 'renameItem'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(68, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 69 }, { QMetaType::QString, 70 },
        }}),
        // Method 'bulkRename'
        QtMocHelpers::MethodData<void(const QStringList &, const QString &, const QString &, const QString &, const QString &)>(71, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 72 }, { QMetaType::QString, 73 }, { QMetaType::QString, 74 }, { QMetaType::QString, 75 },
            { QMetaType::QString, 76 },
        }}),
        // Method 'deleteItem'
        QtMocHelpers::MethodData<void(const QString &)>(77, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'deleteItems'
        QtMocHelpers::MethodData<void(const QStringList &)>(78, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 72 },
        }}),
        // Method 'copyItem'
        QtMocHelpers::MethodData<void(const QString &)>(79, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'cutItem'
        QtMocHelpers::MethodData<void(const QString &)>(80, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'pasteItem'
        QtMocHelpers::MethodData<void()>(81, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'clearClipboard'
        QtMocHelpers::MethodData<void()>(82, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'requestThumbnail'
        QtMocHelpers::MethodData<void(const QString &)>(83, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'selectPath'
        QtMocHelpers::MethodData<void(const QString &)>(84, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'toggleSelection'
        QtMocHelpers::MethodData<void(const QString &)>(85, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'clearSelection'
        QtMocHelpers::MethodData<void()>(86, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'selectAll'
        QtMocHelpers::MethodData<void()>(87, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'selectRangeByIndexes'
        QtMocHelpers::MethodData<void(int, int)>(88, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 89 }, { QMetaType::Int, 90 },
        }}),
        // Method 'pathAtIndex'
        QtMocHelpers::MethodData<QString(int) const>(91, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 92 },
        }}),
        // Method 'nameAtIndex'
        QtMocHelpers::MethodData<QString(int) const>(93, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 92 },
        }}),
        // Method 'indexOfPath'
        QtMocHelpers::MethodData<int(const QString &) const>(94, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'startRename'
        QtMocHelpers::MethodData<void(const QString &)>(95, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'metadataForPath'
        QtMocHelpers::MethodData<QVariantMap(const QString &) const>(96, 2, QMC::AccessPublic, 0x80000000 | 97, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'requestThumbnailsForRange'
        QtMocHelpers::MethodData<void(int, int)>(98, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 99 }, { QMetaType::Int, 100 },
        }}),
        // Method 'getFilePreview'
        QtMocHelpers::MethodData<QString(const QString &)>(101, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'openInTerminal'
        QtMocHelpers::MethodData<void(const QString &)>(102, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'copyToClipboard'
        QtMocHelpers::MethodData<void(const QString &)>(103, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 58 },
        }}),
        // Method 'trashItems'
        QtMocHelpers::MethodData<void(const QStringList &)>(104, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 72 },
        }}),
        // Method 'restoreFromTrash'
        QtMocHelpers::MethodData<void(const QStringList &)>(105, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 72 },
        }}),
        // Method 'emptyTrash'
        QtMocHelpers::MethodData<void()>(106, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'analyseFolder'
        QtMocHelpers::MethodData<void(const QString &)>(107, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'runGitCommand'
        QtMocHelpers::MethodData<QString(const QString &, const QString &)>(108, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 109 }, { QMetaType::QString, 14 },
        }}),
        // Method 'getFolderMetadata'
        QtMocHelpers::MethodData<QVariantMap(const QString &)>(110, 2, QMC::AccessPublic, 0x80000000 | 97, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'requestFolderSize'
        QtMocHelpers::MethodData<void(const QString &)>(111, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'compressItems'
        QtMocHelpers::MethodData<void(const QStringList &)>(112, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 72 },
        }}),
        // Method 'extractItem'
        QtMocHelpers::MethodData<void(const QString &)>(113, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'connectRemote'
        QtMocHelpers::MethodData<void(const QString &)>(114, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 115 },
        }}),
        // Method 'openInCode'
        QtMocHelpers::MethodData<void(const QString &)>(116, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'addToBookmarks'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(117, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 66 },
        }}),
        // Method 'removeFromBookmarks'
        QtMocHelpers::MethodData<void(int)>(118, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 92 },
        }}),
        // Method 'removeBookmarkByPath'
        QtMocHelpers::MethodData<void(const QString &)>(119, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'isBookmarked'
        QtMocHelpers::MethodData<bool(const QString &) const>(120, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'toggleBookmark'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(121, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 66 },
        }}),
        // Method 'openAsRoot'
        QtMocHelpers::MethodData<void(const QString &)>(122, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'openInNewWindow'
        QtMocHelpers::MethodData<void(const QString &)>(123, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'computeChecksum'
        QtMocHelpers::MethodData<QString(const QString &)>(124, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'duplicateItem'
        QtMocHelpers::MethodData<void(const QString &)>(125, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'createSymlink'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(126, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 127 }, { QMetaType::QString, 128 },
        }}),
        // Method 'makeExecutable'
        QtMocHelpers::MethodData<void(const QString &)>(129, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'setPermissions'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(130, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 41 },
        }}),
        // Method 'setWallpaper'
        QtMocHelpers::MethodData<void(const QString &)>(131, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'mountRemote'
        QtMocHelpers::MethodData<void(const QString &)>(132, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 115 },
        }}),
        // Method 'dropItems'
        QtMocHelpers::MethodData<void(const QStringList &, const QString &, bool)>(133, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 72 }, { QMetaType::QString, 134 }, { QMetaType::Bool, 135 },
        }}),
        // Method 'setThumbnailManager'
        QtMocHelpers::MethodData<void(ThumbnailManager *)>(136, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 137, 138 },
        }}),
        // Method 'setPlacesModel'
        QtMocHelpers::MethodData<void(PlacesModel *)>(139, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 140, 141 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'fileModel'
        QtMocHelpers::PropertyData<QObject*>(142, QMetaType::QObjectStar, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'treeModel'
        QtMocHelpers::PropertyData<QObject*>(143, QMetaType::QObjectStar, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'placesModel'
        QtMocHelpers::PropertyData<QObject*>(144, QMetaType::QObjectStar, QMC::DefaultPropertyFlags, 15),
        // property 'currentPath'
        QtMocHelpers::PropertyData<QString>(145, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'loading'
        QtMocHelpers::PropertyData<bool>(146, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'selectedPath'
        QtMocHelpers::PropertyData<QString>(147, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'selectedPaths'
        QtMocHelpers::PropertyData<QStringList>(148, QMetaType::QStringList, QMC::DefaultPropertyFlags, 3),
        // property 'selectionRevision'
        QtMocHelpers::PropertyData<int>(149, QMetaType::Int, QMC::DefaultPropertyFlags, 3),
        // property 'hasSelection'
        QtMocHelpers::PropertyData<bool>(150, QMetaType::Bool, QMC::DefaultPropertyFlags, 3),
        // property 'canGoBack'
        QtMocHelpers::PropertyData<bool>(151, QMetaType::Bool, QMC::DefaultPropertyFlags, 4),
        // property 'canGoForward'
        QtMocHelpers::PropertyData<bool>(152, QMetaType::Bool, QMC::DefaultPropertyFlags, 5),
        // property 'homePath'
        QtMocHelpers::PropertyData<QString>(153, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'title'
        QtMocHelpers::PropertyData<QString>(154, QMetaType::QString, QMC::DefaultPropertyFlags, 8),
        // property 'showHiddenFiles'
        QtMocHelpers::PropertyData<bool>(155, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'hasClipboard'
        QtMocHelpers::PropertyData<bool>(156, QMetaType::Bool, QMC::DefaultPropertyFlags, 7),
        // property 'clipboardPaths'
        QtMocHelpers::PropertyData<QStringList>(157, QMetaType::QStringList, QMC::DefaultPropertyFlags, 7),
        // property 'isCutOp'
        QtMocHelpers::PropertyData<bool>(158, QMetaType::Bool, QMC::DefaultPropertyFlags, 7),
        // property 'iconSize'
        QtMocHelpers::PropertyData<int>(159, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 9),
        // property 'viewMode'
        QtMocHelpers::PropertyData<QString>(39, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 10),
        // property 'availableExtensions'
        QtMocHelpers::PropertyData<QStringList>(160, QMetaType::QStringList, QMC::DefaultPropertyFlags, 12),
        // property 'gitStatus'
        QtMocHelpers::PropertyData<QVariantMap>(161, 0x80000000 | 97, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 13),
        // property 'bookmarksRevision'
        QtMocHelpers::PropertyData<int>(162, QMetaType::Int, QMC::DefaultPropertyFlags, 16),
        // property 'searchMode'
        QtMocHelpers::PropertyData<QString>(163, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 17),
        // property 'searchScope'
        QtMocHelpers::PropertyData<QString>(164, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 18),
        // property 'searchInProgress'
        QtMocHelpers::PropertyData<bool>(165, QMetaType::Bool, QMC::DefaultPropertyFlags, 19),
        // property 'activeSearchTerm'
        QtMocHelpers::PropertyData<QString>(166, QMetaType::QString, QMC::DefaultPropertyFlags, 21),
        // property 'searchContent'
        QtMocHelpers::PropertyData<bool>(167, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 20),
        // property 'recentSearches'
        QtMocHelpers::PropertyData<QStringList>(168, QMetaType::QStringList, QMC::DefaultPropertyFlags, 22),
        // property 'canUndo'
        QtMocHelpers::PropertyData<bool>(169, QMetaType::Bool, QMC::DefaultPropertyFlags, 25),
        // property 'canRedo'
        QtMocHelpers::PropertyData<bool>(170, QMetaType::Bool, QMC::DefaultPropertyFlags, 26),
        // property 'undoDescription'
        QtMocHelpers::PropertyData<QString>(171, QMetaType::QString, QMC::DefaultPropertyFlags, 25),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AppController, qt_meta_tag_ZN13AppControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AppController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13AppControllerE_t>.metaTypes,
    nullptr
} };

void AppController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->currentPathChanged(); break;
        case 1: _t->loadingChanged(); break;
        case 2: _t->selectedPathChanged(); break;
        case 3: _t->selectedPathsChanged(); break;
        case 4: _t->canGoBackChanged(); break;
        case 5: _t->canGoForwardChanged(); break;
        case 6: _t->showHiddenFilesChanged(); break;
        case 7: _t->clipboardChanged(); break;
        case 8: _t->titleChanged(); break;
        case 9: _t->iconSizeChanged(); break;
        case 10: _t->viewModeChanged(); break;
        case 11: _t->renameRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->availableExtensionsChanged(); break;
        case 13: _t->gitStatusChanged(); break;
        case 14: _t->analyseRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->placesModelChanged(); break;
        case 16: _t->bookmarksRevisionChanged(); break;
        case 17: _t->searchModeChanged(); break;
        case 18: _t->searchScopeChanged(); break;
        case 19: _t->searchInProgressChanged(); break;
        case 20: _t->searchContentChanged(); break;
        case 21: _t->activeSearchTermChanged(); break;
        case 22: _t->recentSearchesChanged(); break;
        case 23: _t->requestSearchClear(); break;
        case 24: _t->openWithRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->canUndoChanged(); break;
        case 26: _t->canRedoChanged(); break;
        case 27: _t->folderSizeResolved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[2]))); break;
        case 28: _t->operationError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->operationSuccess((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 30: _t->operationProgress((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 31: _t->undo(); break;
        case 32: _t->redo(); break;
        case 33: { QString _r = _t->viewMode();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 34: _t->setViewMode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 35: _t->openPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->openWith((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: _t->openWithApp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 38: { QVariantList _r = _t->getAssociatedApps((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 39: { QVariantList _r = _t->getAllApplications();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 40: _t->setDefaultApp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 41: { QString _r = _t->getMimeType((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 42: _t->goUp(); break;
        case 43: _t->goBack(); break;
        case 44: _t->goForward(); break;
        case 45: _t->refresh(); break;
        case 46: _t->setSearchText((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 47: _t->startGlobalSearch((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 48: _t->applySearchQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 49: _t->saveSearchQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 50: _t->cancelSearch(); break;
        case 51: _t->createFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 52: _t->createFile((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 53: _t->renameItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 54: _t->bulkRename((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5]))); break;
        case 55: _t->deleteItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 56: _t->deleteItems((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 57: _t->copyItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 58: _t->cutItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 59: _t->pasteItem(); break;
        case 60: _t->clearClipboard(); break;
        case 61: _t->requestThumbnail((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 62: _t->selectPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 63: _t->toggleSelection((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 64: _t->clearSelection(); break;
        case 65: _t->selectAll(); break;
        case 66: _t->selectRangeByIndexes((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 67: { QString _r = _t->pathAtIndex((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 68: { QString _r = _t->nameAtIndex((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 69: { int _r = _t->indexOfPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 70: _t->startRename((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 71: { QVariantMap _r = _t->metadataForPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 72: _t->requestThumbnailsForRange((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 73: { QString _r = _t->getFilePreview((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 74: _t->openInTerminal((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 75: _t->copyToClipboard((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 76: _t->trashItems((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 77: _t->restoreFromTrash((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 78: _t->emptyTrash(); break;
        case 79: _t->analyseFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 80: { QString _r = _t->runGitCommand((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 81: { QVariantMap _r = _t->getFolderMetadata((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 82: _t->requestFolderSize((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 83: _t->compressItems((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 84: _t->extractItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 85: _t->connectRemote((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 86: _t->openInCode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 87: _t->addToBookmarks((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 88: _t->removeFromBookmarks((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 89: _t->removeBookmarkByPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 90: { bool _r = _t->isBookmarked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 91: _t->toggleBookmark((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 92: _t->openAsRoot((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 93: _t->openInNewWindow((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 94: { QString _r = _t->computeChecksum((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 95: _t->duplicateItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 96: _t->createSymlink((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 97: _t->makeExecutable((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 98: _t->setPermissions((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 99: _t->setWallpaper((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 100: _t->mountRemote((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 101: _t->dropItems((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 102: _t->setThumbnailManager((*reinterpret_cast<std::add_pointer_t<ThumbnailManager*>>(_a[1]))); break;
        case 103: _t->setPlacesModel((*reinterpret_cast<std::add_pointer_t<PlacesModel*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 102:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ThumbnailManager* >(); break;
            }
            break;
        case 103:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PlacesModel* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::currentPathChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::loadingChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::selectedPathChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::selectedPathsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::canGoBackChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::canGoForwardChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::showHiddenFilesChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::clipboardChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::titleChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::iconSizeChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::viewModeChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::renameRequested, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::availableExtensionsChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::gitStatusChanged, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::analyseRequested, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::placesModelChanged, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::bookmarksRevisionChanged, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::searchModeChanged, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::searchScopeChanged, 18))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::searchInProgressChanged, 19))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::searchContentChanged, 20))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::activeSearchTermChanged, 21))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::recentSearchesChanged, 22))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::requestSearchClear, 23))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::openWithRequested, 24))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::canUndoChanged, 25))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::canRedoChanged, 26))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & , qlonglong )>(_a, &AppController::folderSizeResolved, 27))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::operationError, 28))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::operationSuccess, 29))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(float )>(_a, &AppController::operationProgress, 30))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QObject**>(_v) = _t->fileModel(); break;
        case 1: *reinterpret_cast<QObject**>(_v) = _t->treeModel(); break;
        case 2: *reinterpret_cast<QObject**>(_v) = _t->placesModel(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->currentPath(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->loading(); break;
        case 5: *reinterpret_cast<QString*>(_v) = _t->selectedPath(); break;
        case 6: *reinterpret_cast<QStringList*>(_v) = _t->selectedPaths(); break;
        case 7: *reinterpret_cast<int*>(_v) = _t->selectionRevision(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->hasSelection(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->canGoBack(); break;
        case 10: *reinterpret_cast<bool*>(_v) = _t->canGoForward(); break;
        case 11: *reinterpret_cast<QString*>(_v) = _t->homePath(); break;
        case 12: *reinterpret_cast<QString*>(_v) = _t->title(); break;
        case 13: *reinterpret_cast<bool*>(_v) = _t->showHiddenFiles(); break;
        case 14: *reinterpret_cast<bool*>(_v) = _t->hasClipboard(); break;
        case 15: *reinterpret_cast<QStringList*>(_v) = _t->clipboardPaths(); break;
        case 16: *reinterpret_cast<bool*>(_v) = _t->isCutOp(); break;
        case 17: *reinterpret_cast<int*>(_v) = _t->iconSize(); break;
        case 18: *reinterpret_cast<QString*>(_v) = _t->viewMode(); break;
        case 19: *reinterpret_cast<QStringList*>(_v) = _t->availableExtensions(); break;
        case 20: *reinterpret_cast<QVariantMap*>(_v) = _t->gitStatus(); break;
        case 21: *reinterpret_cast<int*>(_v) = _t->bookmarksRevision(); break;
        case 22: *reinterpret_cast<QString*>(_v) = _t->searchMode(); break;
        case 23: *reinterpret_cast<QString*>(_v) = _t->searchScope(); break;
        case 24: *reinterpret_cast<bool*>(_v) = _t->searchInProgress(); break;
        case 25: *reinterpret_cast<QString*>(_v) = _t->activeSearchTerm(); break;
        case 26: *reinterpret_cast<bool*>(_v) = _t->searchContent(); break;
        case 27: *reinterpret_cast<QStringList*>(_v) = _t->recentSearches(); break;
        case 28: *reinterpret_cast<bool*>(_v) = _t->canUndo(); break;
        case 29: *reinterpret_cast<bool*>(_v) = _t->canRedo(); break;
        case 30: *reinterpret_cast<QString*>(_v) = _t->undoDescription(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 13: _t->setShowHiddenFiles(*reinterpret_cast<bool*>(_v)); break;
        case 17: _t->setIconSize(*reinterpret_cast<int*>(_v)); break;
        case 18: _t->setViewMode(*reinterpret_cast<QString*>(_v)); break;
        case 22: _t->setSearchMode(*reinterpret_cast<QString*>(_v)); break;
        case 23: _t->setSearchScope(*reinterpret_cast<QString*>(_v)); break;
        case 26: _t->setSearchContent(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *AppController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 104)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 104;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 104)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 104;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    return _id;
}

// SIGNAL 0
void AppController::currentPathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AppController::loadingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppController::selectedPathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppController::selectedPathsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppController::canGoBackChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppController::canGoForwardChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void AppController::showHiddenFilesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void AppController::clipboardChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void AppController::titleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void AppController::iconSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void AppController::viewModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void AppController::renameRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void AppController::availableExtensionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void AppController::gitStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void AppController::analyseRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}

// SIGNAL 15
void AppController::placesModelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void AppController::bookmarksRevisionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void AppController::searchModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void AppController::searchScopeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void AppController::searchInProgressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void AppController::searchContentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 20, nullptr);
}

// SIGNAL 21
void AppController::activeSearchTermChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void AppController::recentSearchesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 22, nullptr);
}

// SIGNAL 23
void AppController::requestSearchClear()
{
    QMetaObject::activate(this, &staticMetaObject, 23, nullptr);
}

// SIGNAL 24
void AppController::openWithRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 24, nullptr, _t1);
}

// SIGNAL 25
void AppController::canUndoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 25, nullptr);
}

// SIGNAL 26
void AppController::canRedoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 26, nullptr);
}

// SIGNAL 27
void AppController::folderSizeResolved(const QString & _t1, qlonglong _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 27, nullptr, _t1, _t2);
}

// SIGNAL 28
void AppController::operationError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 28, nullptr, _t1);
}

// SIGNAL 29
void AppController::operationSuccess(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 29, nullptr, _t1);
}

// SIGNAL 30
void AppController::operationProgress(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 30, nullptr, _t1);
}
QT_WARNING_POP
