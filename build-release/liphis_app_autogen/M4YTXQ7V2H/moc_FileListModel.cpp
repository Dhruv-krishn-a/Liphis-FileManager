/****************************************************************************
** Meta object code from reading C++ file 'FileListModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/models/FileListModel.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileListModel.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13FileListModelE_t {};
} // unnamed namespace

template <> constexpr inline auto FileListModel::qt_create_metaobjectdata<qt_meta_tag_ZN13FileListModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FileListModel",
        "countChanged",
        "",
        "sortFieldChanged",
        "sortAscendingChanged",
        "metadataForPath",
        "QVariantMap",
        "path",
        "availableExtensions",
        "setSortBy",
        "field",
        "ascending",
        "count",
        "sortField",
        "sortAscending"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'countChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sortFieldChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sortAscendingChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'metadataForPath'
        QtMocHelpers::MethodData<QVariantMap(const QString &) const>(5, 2, QMC::AccessPublic, 0x80000000 | 6, {{
            { QMetaType::QString, 7 },
        }}),
        // Method 'availableExtensions'
        QtMocHelpers::MethodData<QStringList() const>(8, 2, QMC::AccessPublic, QMetaType::QStringList),
        // Method 'setSortBy'
        QtMocHelpers::MethodData<void(const QString &, bool)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::Bool, 11 },
        }}),
        // Method 'setSortBy'
        QtMocHelpers::MethodData<void(const QString &)>(9, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'count'
        QtMocHelpers::PropertyData<int>(12, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'sortField'
        QtMocHelpers::PropertyData<QString>(13, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'sortAscending'
        QtMocHelpers::PropertyData<bool>(14, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FileListModel, qt_meta_tag_ZN13FileListModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FileListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13FileListModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13FileListModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13FileListModelE_t>.metaTypes,
    nullptr
} };

void FileListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FileListModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->sortFieldChanged(); break;
        case 2: _t->sortAscendingChanged(); break;
        case 3: { QVariantMap _r = _t->metadataForPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 4: { QStringList _r = _t->availableExtensions();
            if (_a[0]) *reinterpret_cast<QStringList*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->setSortBy((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->setSortBy((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FileListModel::*)()>(_a, &FileListModel::countChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileListModel::*)()>(_a, &FileListModel::sortFieldChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileListModel::*)()>(_a, &FileListModel::sortAscendingChanged, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->rowCount(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->sortField(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->sortAscending(); break;
        default: break;
        }
    }
}

const QMetaObject *FileListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13FileListModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int FileListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FileListModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FileListModel::sortFieldChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void FileListModel::sortAscendingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
