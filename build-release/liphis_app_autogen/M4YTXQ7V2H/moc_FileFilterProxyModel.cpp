/****************************************************************************
** Meta object code from reading C++ file 'FileFilterProxyModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
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
        "showHidden",
        "minSize",
        "maxSize",
        "minDate",
        "maxDate",
        "extensionFilter"
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
    };
    QtMocHelpers::UintData qt_properties {
        // property 'showHidden'
        QtMocHelpers::PropertyData<bool>(8, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'minSize'
        QtMocHelpers::PropertyData<qlonglong>(9, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'maxSize'
        QtMocHelpers::PropertyData<qlonglong>(10, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'minDate'
        QtMocHelpers::PropertyData<qlonglong>(11, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'maxDate'
        QtMocHelpers::PropertyData<qlonglong>(12, QMetaType::LongLong, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'extensionFilter'
        QtMocHelpers::PropertyData<QString>(13, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
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
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
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
QT_WARNING_POP
