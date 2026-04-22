/****************************************************************************
** Meta object code from reading C++ file 'DocumentIntelligenceController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/DocumentIntelligenceController.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocumentIntelligenceController.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN30DocumentIntelligenceControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto DocumentIntelligenceController::qt_create_metaobjectdata<qt_meta_tag_ZN30DocumentIntelligenceControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DocumentIntelligenceController",
        "enabledChanged",
        "",
        "busyChanged",
        "lastScanRootChanged",
        "groupsChanged",
        "inboxItemsChanged",
        "duplicatesChanged",
        "healthReportChanged",
        "namingPreviewChanged",
        "operationNotice",
        "message",
        "isError",
        "initialize",
        "ensureInitialized",
        "scanFolder",
        "path",
        "scanDownloadsInbox",
        "timelineForPath",
        "QVariantMap",
        "groupForPath",
        "setCurrentVersion",
        "groupId",
        "setDocumentStatus",
        "status",
        "triageInbox",
        "action",
        "clearInbox",
        "previewCanonicalNames",
        "QVariantList",
        "rootPath",
        "nameTemplate",
        "applyCanonicalNames",
        "undoLastCanonicalRename",
        "mergeExactDuplicateGroup",
        "hashKey",
        "defaultNameTemplate",
        "enabled",
        "busy",
        "lastScanRoot",
        "groups",
        "inboxItems",
        "duplicateExact",
        "duplicateNear",
        "healthReport",
        "namingPreview",
        "downloadsPath"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'enabledChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastScanRootChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'groupsChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'inboxItemsChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'duplicatesChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'healthReportChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'namingPreviewChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'operationNotice'
        QtMocHelpers::SignalData<void(const QString &, bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 }, { QMetaType::Bool, 12 },
        }}),
        // Method 'initialize'
        QtMocHelpers::MethodData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'ensureInitialized'
        QtMocHelpers::MethodData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'scanFolder'
        QtMocHelpers::MethodData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Method 'scanDownloadsInbox'
        QtMocHelpers::MethodData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'timelineForPath'
        QtMocHelpers::MethodData<QVariantMap(const QString &) const>(18, 2, QMC::AccessPublic, 0x80000000 | 19, {{
            { QMetaType::QString, 16 },
        }}),
        // Method 'groupForPath'
        QtMocHelpers::MethodData<QVariantMap(const QString &) const>(20, 2, QMC::AccessPublic, 0x80000000 | 19, {{
            { QMetaType::QString, 16 },
        }}),
        // Method 'setCurrentVersion'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 22 }, { QMetaType::QString, 16 },
        }}),
        // Method 'setDocumentStatus'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 }, { QMetaType::QString, 24 },
        }}),
        // Method 'triageInbox'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 }, { QMetaType::QString, 26 },
        }}),
        // Method 'clearInbox'
        QtMocHelpers::MethodData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'previewCanonicalNames'
        QtMocHelpers::MethodData<QVariantList(const QString &, const QString &)>(28, 2, QMC::AccessPublic, 0x80000000 | 29, {{
            { QMetaType::QString, 30 }, { QMetaType::QString, 31 },
        }}),
        // Method 'applyCanonicalNames'
        QtMocHelpers::MethodData<int()>(32, 2, QMC::AccessPublic, QMetaType::Int),
        // Method 'undoLastCanonicalRename'
        QtMocHelpers::MethodData<int()>(33, 2, QMC::AccessPublic, QMetaType::Int),
        // Method 'mergeExactDuplicateGroup'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 35 }, { QMetaType::QString, 26 },
        }}),
        // Method 'defaultNameTemplate'
        QtMocHelpers::MethodData<QString() const>(36, 2, QMC::AccessPublic, QMetaType::QString),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'enabled'
        QtMocHelpers::PropertyData<bool>(37, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(38, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'lastScanRoot'
        QtMocHelpers::PropertyData<QString>(39, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'groups'
        QtMocHelpers::PropertyData<QVariantList>(40, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 3),
        // property 'inboxItems'
        QtMocHelpers::PropertyData<QVariantList>(41, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 4),
        // property 'duplicateExact'
        QtMocHelpers::PropertyData<QVariantList>(42, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 5),
        // property 'duplicateNear'
        QtMocHelpers::PropertyData<QVariantList>(43, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 5),
        // property 'healthReport'
        QtMocHelpers::PropertyData<QVariantMap>(44, 0x80000000 | 19, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 6),
        // property 'namingPreview'
        QtMocHelpers::PropertyData<QVariantList>(45, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 7),
        // property 'downloadsPath'
        QtMocHelpers::PropertyData<QString>(46, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DocumentIntelligenceController, qt_meta_tag_ZN30DocumentIntelligenceControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DocumentIntelligenceController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN30DocumentIntelligenceControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN30DocumentIntelligenceControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN30DocumentIntelligenceControllerE_t>.metaTypes,
    nullptr
} };

void DocumentIntelligenceController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DocumentIntelligenceController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->enabledChanged(); break;
        case 1: _t->busyChanged(); break;
        case 2: _t->lastScanRootChanged(); break;
        case 3: _t->groupsChanged(); break;
        case 4: _t->inboxItemsChanged(); break;
        case 5: _t->duplicatesChanged(); break;
        case 6: _t->healthReportChanged(); break;
        case 7: _t->namingPreviewChanged(); break;
        case 8: _t->operationNotice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 9: _t->initialize(); break;
        case 10: _t->ensureInitialized(); break;
        case 11: _t->scanFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->scanDownloadsInbox(); break;
        case 13: { QVariantMap _r = _t->timelineForPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 14: { QVariantMap _r = _t->groupForPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 15: _t->setCurrentVersion((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 16: _t->setDocumentStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 17: _t->triageInbox((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 18: _t->clearInbox(); break;
        case 19: { QVariantList _r = _t->previewCanonicalNames((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 20: { int _r = _t->applyCanonicalNames();
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 21: { int _r = _t->undoLastCanonicalRename();
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 22: _t->mergeExactDuplicateGroup((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 23: { QString _r = _t->defaultNameTemplate();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::enabledChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::busyChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::lastScanRootChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::groupsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::inboxItemsChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::duplicatesChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::healthReportChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)()>(_a, &DocumentIntelligenceController::namingPreviewChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentIntelligenceController::*)(const QString & , bool )>(_a, &DocumentIntelligenceController::operationNotice, 8))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->enabled(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->busy(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->lastScanRoot(); break;
        case 3: *reinterpret_cast<QVariantList*>(_v) = _t->groups(); break;
        case 4: *reinterpret_cast<QVariantList*>(_v) = _t->inboxItems(); break;
        case 5: *reinterpret_cast<QVariantList*>(_v) = _t->duplicateExact(); break;
        case 6: *reinterpret_cast<QVariantList*>(_v) = _t->duplicateNear(); break;
        case 7: *reinterpret_cast<QVariantMap*>(_v) = _t->healthReport(); break;
        case 8: *reinterpret_cast<QVariantList*>(_v) = _t->namingPreview(); break;
        case 9: *reinterpret_cast<QString*>(_v) = _t->downloadsPath(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setEnabled(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *DocumentIntelligenceController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocumentIntelligenceController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN30DocumentIntelligenceControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DocumentIntelligenceController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void DocumentIntelligenceController::enabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DocumentIntelligenceController::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DocumentIntelligenceController::lastScanRootChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DocumentIntelligenceController::groupsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DocumentIntelligenceController::inboxItemsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void DocumentIntelligenceController::duplicatesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void DocumentIntelligenceController::healthReportChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void DocumentIntelligenceController::namingPreviewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void DocumentIntelligenceController::operationNotice(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}
QT_WARNING_POP
