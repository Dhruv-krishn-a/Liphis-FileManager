/****************************************************************************
** Meta object code from reading C++ file 'TerminalManager.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/TerminalManager.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TerminalManager.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15TerminalManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto TerminalManager::qt_create_metaobjectdata<qt_meta_tag_ZN15TerminalManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TerminalManager",
        "outputChanged",
        "",
        "currentDirChanged",
        "commandExecuted",
        "cmd",
        "readOutput",
        "handleError",
        "QProcess::ProcessError",
        "error",
        "sendCommand",
        "clear",
        "output",
        "currentDir"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'outputChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'currentDirChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'commandExecuted'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'readOutput'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleError'
        QtMocHelpers::SlotData<void(QProcess::ProcessError)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Method 'sendCommand'
        QtMocHelpers::MethodData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'clear'
        QtMocHelpers::MethodData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'output'
        QtMocHelpers::PropertyData<QString>(12, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'currentDir'
        QtMocHelpers::PropertyData<QString>(13, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TerminalManager, qt_meta_tag_ZN15TerminalManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TerminalManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15TerminalManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15TerminalManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15TerminalManagerE_t>.metaTypes,
    nullptr
} };

void TerminalManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TerminalManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->outputChanged(); break;
        case 1: _t->currentDirChanged(); break;
        case 2: _t->commandExecuted((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->readOutput(); break;
        case 4: _t->handleError((*reinterpret_cast<std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 5: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->clear(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TerminalManager::*)()>(_a, &TerminalManager::outputChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TerminalManager::*)()>(_a, &TerminalManager::currentDirChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TerminalManager::*)(const QString & )>(_a, &TerminalManager::commandExecuted, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->output(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->currentDir(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setCurrentDir(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *TerminalManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TerminalManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15TerminalManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TerminalManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void TerminalManager::outputChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TerminalManager::currentDirChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TerminalManager::commandExecuted(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
