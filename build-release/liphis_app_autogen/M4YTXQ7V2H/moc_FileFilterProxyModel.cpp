/****************************************************************************
** Meta object code from reading C++ file 'FileFilterProxyModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/models/FileFilterProxyModel.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileFilterProxyModel.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20FileFilterProxyModelE_t {};
} // unnamed namespace

template <> constexpr inline auto FileFilterProxyModel::qt_create_metaobjectdata<qt_meta_tag_ZN20FileFilterProxyModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FileFilterProxyModel",
        "showHiddenChanged",
        "",
        "minSizeChanged",
        "maxSizeChanged",
        "minDateChanged",
        "maxDateChanged",
        "extensionFilterChanged",
        "searchQueryChanged",
        "typeFilterChanged",
        "exactMatchChanged",
        "countChanged",
        "setSortBy",
        "roleName",
        "ascending",
        "showHidden",
        "minSize",
        "maxSize",
        "minDate",
        "maxDate",
        "extensionFilter",
        "searchQuery",
        "typeFilter",
        "exactMatch",
        "count"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'showHiddenChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'minSizeChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'maxSizeChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'minDateChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'maxDateChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'extensionFilterChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchQueryChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'typeFilterChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'exactMatchChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'countChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'setSortBy'
        QtMocHelpers::MethodData<void(const QString &, bool)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::Bool, 14 },
        }}),
        // Method 'setSortBy'
        QtMocHelpers::MethodData<void(const QString &)>(12, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'showHidden'
        QtMocHelpers::PropertyData<bool>(15, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'minSize'
        QtMocHelpers::PropertyData<qlonglong>(16, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'maxSize'
        QtMocHelpers::PropertyData<qlonglong>(17, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'minDate'
        QtMocHelpers::PropertyData<qlonglong>(18, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'maxDate'
        QtMocHelpers::PropertyData<qlonglong>(19, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'extensionFilter'
        QtMocHelpers::PropertyData<QString>(20, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'searchQuery'
        QtMocHelpers::PropertyData<QString>(21, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'typeFilter'
        QtMocHelpers::PropertyData<QString>(22, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'exactMatch'
        QtMocHelpers::PropertyData<bool>(23, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 8),
        // property 'count'
        QtMocHelpers::PropertyData<int>(24, QMetaType::Int, QMC::DefaultPropertyFlags, 9),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FileFilterProxyModel, qt_meta_tag_ZN20FileFilterProxyModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FileFilterProxyModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QSortFilterProxyModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20FileFilterProxyModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20FileFilterProxyModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20FileFilterProxyModelE_t>.metaTypes,
    nullptr
} };

void FileFilterProxyModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FileFilterProxyModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->showHiddenChanged(); break;
        case 1: _t->minSizeChanged(); break;
        case 2: _t->maxSizeChanged(); break;
        case 3: _t->minDateChanged(); break;
        case 4: _t->maxDateChanged(); break;
        case 5: _t->extensionFilterChanged(); break;
        case 6: _t->searchQueryChanged(); break;
        case 7: _t->typeFilterChanged(); break;
        case 8: _t->exactMatchChanged(); break;
        case 9: _t->countChanged(); break;
        case 10: _t->setSortBy((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 11: _t->setSortBy((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::showHiddenChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::minSizeChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::maxSizeChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::minDateChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::maxDateChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::extensionFilterChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::searchQueryChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::typeFilterChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::exactMatchChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileFilterProxyModel::*)()>(_a, &FileFilterProxyModel::countChanged, 9))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->showHidden(); break;
        case 1: *reinterpret_cast<qlonglong*>(_v) = _t->minSize(); break;
        case 2: *reinterpret_cast<qlonglong*>(_v) = _t->maxSize(); break;
        case 3: *reinterpret_cast<qlonglong*>(_v) = _t->minDate(); break;
        case 4: *reinterpret_cast<qlonglong*>(_v) = _t->maxDate(); break;
        case 5: *reinterpret_cast<QString*>(_v) = _t->extensionFilter(); break;
        case 6: *reinterpret_cast<QString*>(_v) = _t->searchQuery(); break;
        case 7: *reinterpret_cast<QString*>(_v) = _t->typeFilter(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->exactMatch(); break;
        case 9: *reinterpret_cast<int*>(_v) = _t->rowCount(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setShowHidden(*reinterpret_cast<bool*>(_v)); break;
        case 1: _t->setMinSize(*reinterpret_cast<qlonglong*>(_v)); break;
        case 2: _t->setMaxSize(*reinterpret_cast<qlonglong*>(_v)); break;
        case 3: _t->setMinDate(*reinterpret_cast<qlonglong*>(_v)); break;
        case 4: _t->setMaxDate(*reinterpret_cast<qlonglong*>(_v)); break;
        case 5: _t->setExtensionFilter(*reinterpret_cast<QString*>(_v)); break;
        case 6: _t->setSearchQuery(*reinterpret_cast<QString*>(_v)); break;
        case 7: _t->setTypeFilter(*reinterpret_cast<QString*>(_v)); break;
        case 8: _t->setExactMatch(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *FileFilterProxyModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileFilterProxyModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20FileFilterProxyModelE_t>.strings))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int FileFilterProxyModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
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
void FileFilterProxyModel::showHiddenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FileFilterProxyModel::minSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void FileFilterProxyModel::maxSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void FileFilterProxyModel::minDateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void FileFilterProxyModel::maxDateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void FileFilterProxyModel::extensionFilterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void FileFilterProxyModel::searchQueryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void FileFilterProxyModel::typeFilterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void FileFilterProxyModel::exactMatchChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void FileFilterProxyModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
