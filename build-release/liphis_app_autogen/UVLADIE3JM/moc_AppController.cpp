/****************************************************************************
** Meta object code from reading C++ file 'AppController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
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
#error "This file was generated using the moc from 6.10.2. It"
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
        "operationError",
        "message",
        "operationSuccess",
        "operationProgress",
        "progress",
        "openPath",
        "goUp",
        "goBack",
        "goForward",
        "refresh",
        "setSearchText",
        "text",
        "startGlobalSearch",
        "pattern",
        "createFolder",
        "name",
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
        "copyItem",
        "cutItem",
        "pasteItem",
        "requestThumbnail",
        "selectPath",
        "toggleSelection",
        "clearSelection",
        "selectAll",
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
        "analyseFolder",
        "compressItems",
        "extractItem",
        "openInCode",
        "addToBookmarks",
        "removeFromBookmarks",
        "index",
        "openAsRoot",
        "computeChecksum",
        "duplicateItem",
        "createSymlink",
        "target",
        "linkName",
        "makeExecutable",
        "setPermissions",
        "mode",
        "setWallpaper",
        "mountRemote",
        "url",
        "setThumbnailManager",
        "ThumbnailManager*",
        "manager",
        "setPlacesModel",
        "PlacesModel*",
        "model",
        "fileModel",
        "placesModel",
        "currentPath",
        "loading",
        "selectedPath",
        "selectedPaths",
        "hasSelection",
        "canGoBack",
        "canGoForward",
        "homePath",
        "title",
        "showHiddenFiles",
        "hasClipboard",
        "clipboardPath",
        "isCutOp",
        "iconSize",
        "viewMode",
        "availableExtensions",
        "gitStatus"
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
        // Signal 'operationError'
        QtMocHelpers::SignalData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Signal 'operationSuccess'
        QtMocHelpers::SignalData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Signal 'operationProgress'
        QtMocHelpers::SignalData<void(float)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 23 },
        }}),
        // Method 'openPath'
        QtMocHelpers::MethodData<void(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'goUp'
        QtMocHelpers::MethodData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'goBack'
        QtMocHelpers::MethodData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'goForward'
        QtMocHelpers::MethodData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'refresh'
        QtMocHelpers::MethodData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'setSearchText'
        QtMocHelpers::MethodData<void(const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 30 },
        }}),
        // Method 'startGlobalSearch'
        QtMocHelpers::MethodData<void(const QString &)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 32 },
        }}),
        // Method 'createFolder'
        QtMocHelpers::MethodData<void(const QString &)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Method 'renameItem'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 36 }, { QMetaType::QString, 37 },
        }}),
        // Method 'bulkRename'
        QtMocHelpers::MethodData<void(const QStringList &, const QString &, const QString &, const QString &, const QString &)>(38, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 39 }, { QMetaType::QString, 40 }, { QMetaType::QString, 41 }, { QMetaType::QString, 42 },
            { QMetaType::QString, 43 },
        }}),
        // Method 'deleteItem'
        QtMocHelpers::MethodData<void(const QString &)>(44, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'copyItem'
        QtMocHelpers::MethodData<void(const QString &)>(45, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'cutItem'
        QtMocHelpers::MethodData<void(const QString &)>(46, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'pasteItem'
        QtMocHelpers::MethodData<void()>(47, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'requestThumbnail'
        QtMocHelpers::MethodData<void(const QString &)>(48, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'selectPath'
        QtMocHelpers::MethodData<void(const QString &)>(49, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'toggleSelection'
        QtMocHelpers::MethodData<void(const QString &)>(50, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'clearSelection'
        QtMocHelpers::MethodData<void()>(51, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'selectAll'
        QtMocHelpers::MethodData<void()>(52, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'startRename'
        QtMocHelpers::MethodData<void(const QString &)>(53, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'metadataForPath'
        QtMocHelpers::MethodData<QVariantMap(const QString &) const>(54, 2, QMC::AccessPublic, 0x80000000 | 55, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'requestThumbnailsForRange'
        QtMocHelpers::MethodData<void(int, int)>(56, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 57 }, { QMetaType::Int, 58 },
        }}),
        // Method 'getFilePreview'
        QtMocHelpers::MethodData<QString(const QString &)>(59, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'openInTerminal'
        QtMocHelpers::MethodData<void(const QString &)>(60, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'copyToClipboard'
        QtMocHelpers::MethodData<void(const QString &)>(61, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 30 },
        }}),
        // Method 'trashItems'
        QtMocHelpers::MethodData<void(const QStringList &)>(62, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 39 },
        }}),
        // Method 'analyseFolder'
        QtMocHelpers::MethodData<void(const QString &)>(63, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'compressItems'
        QtMocHelpers::MethodData<void(const QStringList &)>(64, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 39 },
        }}),
        // Method 'extractItem'
        QtMocHelpers::MethodData<void(const QString &)>(65, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'openInCode'
        QtMocHelpers::MethodData<void(const QString &)>(66, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'addToBookmarks'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(67, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 34 },
        }}),
        // Method 'removeFromBookmarks'
        QtMocHelpers::MethodData<void(int)>(68, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 69 },
        }}),
        // Method 'openAsRoot'
        QtMocHelpers::MethodData<void(const QString &)>(70, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'computeChecksum'
        QtMocHelpers::MethodData<QString(const QString &)>(71, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'duplicateItem'
        QtMocHelpers::MethodData<void(const QString &)>(72, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'createSymlink'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(73, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 74 }, { QMetaType::QString, 75 },
        }}),
        // Method 'makeExecutable'
        QtMocHelpers::MethodData<void(const QString &)>(76, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'setPermissions'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(77, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 78 },
        }}),
        // Method 'setWallpaper'
        QtMocHelpers::MethodData<void(const QString &)>(79, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'mountRemote'
        QtMocHelpers::MethodData<void(const QString &)>(80, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 81 },
        }}),
        // Method 'setThumbnailManager'
        QtMocHelpers::MethodData<void(ThumbnailManager *)>(82, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 83, 84 },
        }}),
        // Method 'setPlacesModel'
        QtMocHelpers::MethodData<void(PlacesModel *)>(85, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 86, 87 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'fileModel'
        QtMocHelpers::PropertyData<QObject*>(88, QMetaType::QObjectStar, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'placesModel'
        QtMocHelpers::PropertyData<QObject*>(89, QMetaType::QObjectStar, QMC::DefaultPropertyFlags, 15),
        // property 'currentPath'
        QtMocHelpers::PropertyData<QString>(90, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'loading'
        QtMocHelpers::PropertyData<bool>(91, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'selectedPath'
        QtMocHelpers::PropertyData<QString>(92, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'selectedPaths'
        QtMocHelpers::PropertyData<QStringList>(93, QMetaType::QStringList, QMC::DefaultPropertyFlags, 3),
        // property 'hasSelection'
        QtMocHelpers::PropertyData<bool>(94, QMetaType::Bool, QMC::DefaultPropertyFlags, 3),
        // property 'canGoBack'
        QtMocHelpers::PropertyData<bool>(95, QMetaType::Bool, QMC::DefaultPropertyFlags, 4),
        // property 'canGoForward'
        QtMocHelpers::PropertyData<bool>(96, QMetaType::Bool, QMC::DefaultPropertyFlags, 5),
        // property 'homePath'
        QtMocHelpers::PropertyData<QString>(97, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'title'
        QtMocHelpers::PropertyData<QString>(98, QMetaType::QString, QMC::DefaultPropertyFlags, 8),
        // property 'showHiddenFiles'
        QtMocHelpers::PropertyData<bool>(99, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'hasClipboard'
        QtMocHelpers::PropertyData<bool>(100, QMetaType::Bool, QMC::DefaultPropertyFlags, 7),
        // property 'clipboardPath'
        QtMocHelpers::PropertyData<QString>(101, QMetaType::QString, QMC::DefaultPropertyFlags, 7),
        // property 'isCutOp'
        QtMocHelpers::PropertyData<bool>(102, QMetaType::Bool, QMC::DefaultPropertyFlags, 7),
        // property 'iconSize'
        QtMocHelpers::PropertyData<int>(103, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 9),
        // property 'viewMode'
        QtMocHelpers::PropertyData<QString>(104, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 10),
        // property 'availableExtensions'
        QtMocHelpers::PropertyData<QStringList>(105, QMetaType::QStringList, QMC::DefaultPropertyFlags, 12),
        // property 'gitStatus'
        QtMocHelpers::PropertyData<QVariantMap>(106, 0x80000000 | 55, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 13),
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
        case 16: _t->operationError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->operationSuccess((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->operationProgress((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 19: _t->openPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->goUp(); break;
        case 21: _t->goBack(); break;
        case 22: _t->goForward(); break;
        case 23: _t->refresh(); break;
        case 24: _t->setSearchText((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->startGlobalSearch((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->createFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->renameItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 28: _t->bulkRename((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5]))); break;
        case 29: _t->deleteItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 30: _t->copyItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 31: _t->cutItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->pasteItem(); break;
        case 33: _t->requestThumbnail((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 34: _t->selectPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 35: _t->toggleSelection((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->clearSelection(); break;
        case 37: _t->selectAll(); break;
        case 38: _t->startRename((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 39: { QVariantMap _r = _t->metadataForPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 40: _t->requestThumbnailsForRange((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 41: { QString _r = _t->getFilePreview((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 42: _t->openInTerminal((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 43: _t->copyToClipboard((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 44: _t->trashItems((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 45: _t->analyseFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 46: _t->compressItems((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 47: _t->extractItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 48: _t->openInCode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 49: _t->addToBookmarks((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 50: _t->removeFromBookmarks((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 51: _t->openAsRoot((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 52: { QString _r = _t->computeChecksum((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 53: _t->duplicateItem((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 54: _t->createSymlink((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 55: _t->makeExecutable((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 56: _t->setPermissions((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 57: _t->setWallpaper((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 58: _t->mountRemote((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 59: _t->setThumbnailManager((*reinterpret_cast<std::add_pointer_t<ThumbnailManager*>>(_a[1]))); break;
        case 60: _t->setPlacesModel((*reinterpret_cast<std::add_pointer_t<PlacesModel*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 59:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ThumbnailManager* >(); break;
            }
            break;
        case 60:
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
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::operationError, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::operationSuccess, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(float )>(_a, &AppController::operationProgress, 18))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QObject**>(_v) = _t->fileModel(); break;
        case 1: *reinterpret_cast<QObject**>(_v) = _t->placesModel(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->currentPath(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->loading(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->selectedPath(); break;
        case 5: *reinterpret_cast<QStringList*>(_v) = _t->selectedPaths(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->hasSelection(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->canGoBack(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->canGoForward(); break;
        case 9: *reinterpret_cast<QString*>(_v) = _t->homePath(); break;
        case 10: *reinterpret_cast<QString*>(_v) = _t->title(); break;
        case 11: *reinterpret_cast<bool*>(_v) = _t->showHiddenFiles(); break;
        case 12: *reinterpret_cast<bool*>(_v) = _t->hasClipboard(); break;
        case 13: *reinterpret_cast<QString*>(_v) = _t->clipboardPath(); break;
        case 14: *reinterpret_cast<bool*>(_v) = _t->isCutOp(); break;
        case 15: *reinterpret_cast<int*>(_v) = _t->iconSize(); break;
        case 16: *reinterpret_cast<QString*>(_v) = _t->viewMode(); break;
        case 17: *reinterpret_cast<QStringList*>(_v) = _t->availableExtensions(); break;
        case 18: *reinterpret_cast<QVariantMap*>(_v) = _t->gitStatus(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 11: _t->setShowHiddenFiles(*reinterpret_cast<bool*>(_v)); break;
        case 15: _t->setIconSize(*reinterpret_cast<int*>(_v)); break;
        case 16: _t->setViewMode(*reinterpret_cast<QString*>(_v)); break;
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
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
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
void AppController::operationError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 16, nullptr, _t1);
}

// SIGNAL 17
void AppController::operationSuccess(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 17, nullptr, _t1);
}

// SIGNAL 18
void AppController::operationProgress(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 18, nullptr, _t1);
}
QT_WARNING_POP
