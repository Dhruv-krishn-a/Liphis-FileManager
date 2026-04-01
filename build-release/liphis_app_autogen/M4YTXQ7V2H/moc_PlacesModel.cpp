/****************************************************************************
** Meta object code from reading C++ file 'PlacesModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/models/PlacesModel.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlacesModel.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11PlacesModelE_t {};
} // unnamed namespace

template <> constexpr inline auto PlacesModel::qt_create_metaobjectdata<qt_meta_tag_ZN11PlacesModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PlacesModel",
        "refresh",
        "",
        "addBookmark",
        "path",
        "name",
        "removeBookmark",
        "index",
        "removeBookmarkByPath",
        "isBookmarked",
        "addRecent"
    };

    QtMocHelpers::UintData qt_methods {
        // Method 'refresh'
        QtMocHelpers::MethodData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'addBookmark'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 5 },
        }}),
        // Method 'removeBookmark'
        QtMocHelpers::MethodData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Method 'removeBookmarkByPath'
        QtMocHelpers::MethodData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Method 'isBookmarked'
        QtMocHelpers::MethodData<bool(const QString &) const>(9, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 4 },
        }}),
        // Method 'addRecent'
        QtMocHelpers::MethodData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PlacesModel, qt_meta_tag_ZN11PlacesModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PlacesModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11PlacesModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11PlacesModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11PlacesModelE_t>.metaTypes,
    nullptr
} };

void PlacesModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PlacesModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->refresh(); break;
        case 1: _t->addBookmark((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->removeBookmark((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->removeBookmarkByPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: { bool _r = _t->isBookmarked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->addRecent((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *PlacesModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlacesModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11PlacesModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int PlacesModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
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
    return _id;
}
QT_WARNING_POP
