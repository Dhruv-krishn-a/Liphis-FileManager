/****************************************************************************
** Meta object code from reading C++ file 'AnalysisController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/AnalysisController.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisController.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18AnalysisControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto AnalysisController::qt_create_metaobjectdata<qt_meta_tag_ZN18AnalysisControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AnalysisController",
        "busyChanged",
        "",
        "analysisStarted",
        "analysisFinished",
        "QVariantMap",
        "result",
        "nodeFound",
        "node",
        "analysisProgress",
        "size",
        "files",
        "dirs",
        "currentItem",
        "folderSizeCalculated",
        "path",
        "analyseFolder",
        "calculateFolderSize",
        "cancel",
        "copyTreeToClipboard",
        "busy"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'analysisStarted'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'analysisFinished'
        QtMocHelpers::SignalData<void(QVariantMap)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'nodeFound'
        QtMocHelpers::SignalData<void(QVariantMap)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 8 },
        }}),
        // Signal 'analysisProgress'
        QtMocHelpers::SignalData<void(qlonglong, int, int, QString)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 10 }, { QMetaType::Int, 11 }, { QMetaType::Int, 12 }, { QMetaType::QString, 13 },
        }}),
        // Signal 'folderSizeCalculated'
        QtMocHelpers::SignalData<void(QString, qlonglong)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 }, { QMetaType::LongLong, 10 },
        }}),
        // Method 'analyseFolder'
        QtMocHelpers::MethodData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'calculateFolderSize'
        QtMocHelpers::MethodData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'cancel'
        QtMocHelpers::MethodData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'copyTreeToClipboard'
        QtMocHelpers::MethodData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(20, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AnalysisController, qt_meta_tag_ZN18AnalysisControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AnalysisController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18AnalysisControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18AnalysisControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18AnalysisControllerE_t>.metaTypes,
    nullptr
} };

void AnalysisController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AnalysisController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->busyChanged(); break;
        case 1: _t->analysisStarted(); break;
        case 2: _t->analysisFinished((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 3: _t->nodeFound((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 4: _t->analysisProgress((*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 5: _t->folderSizeCalculated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[2]))); break;
        case 6: _t->analyseFolder((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->calculateFolderSize((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->cancel(); break;
        case 9: _t->copyTreeToClipboard(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AnalysisController::*)()>(_a, &AnalysisController::busyChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AnalysisController::*)()>(_a, &AnalysisController::analysisStarted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AnalysisController::*)(QVariantMap )>(_a, &AnalysisController::analysisFinished, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AnalysisController::*)(QVariantMap )>(_a, &AnalysisController::nodeFound, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AnalysisController::*)(qlonglong , int , int , QString )>(_a, &AnalysisController::analysisProgress, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AnalysisController::*)(QString , qlonglong )>(_a, &AnalysisController::folderSizeCalculated, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->busy(); break;
        default: break;
        }
    }
}

const QMetaObject *AnalysisController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AnalysisController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18AnalysisControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AnalysisController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void AnalysisController::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AnalysisController::analysisStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AnalysisController::analysisFinished(QVariantMap _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void AnalysisController::nodeFound(QVariantMap _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void AnalysisController::analysisProgress(qlonglong _t1, int _t2, int _t3, QString _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 5
void AnalysisController::folderSizeCalculated(QString _t1, qlonglong _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}
QT_WARNING_POP
