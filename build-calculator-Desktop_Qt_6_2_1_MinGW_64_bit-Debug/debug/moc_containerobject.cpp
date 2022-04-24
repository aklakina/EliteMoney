/****************************************************************************
** Meta object code from reading C++ file 'containerobject.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../SRC/containerobject.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'containerobject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ContainerObject_t {
    const uint offsetsAndSize[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ContainerObject_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ContainerObject_t qt_meta_stringdata_ContainerObject = {
    {
QT_MOC_LITERAL(0, 15) // "ContainerObject"

    },
    "ContainerObject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ContainerObject[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ContainerObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject ContainerObject::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ContainerObject.offsetsAndSize,
    qt_meta_data_ContainerObject,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ContainerObject_t
, QtPrivate::TypeAndForceComplete<ContainerObject, std::true_type>



>,
    nullptr
} };


const QMetaObject *ContainerObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ContainerObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ContainerObject.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ContainerObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_faction_t {
    const uint offsetsAndSize[10];
    char stringdata0[33];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_faction_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_faction_t qt_meta_stringdata_faction = {
    {
QT_MOC_LITERAL(0, 7), // "faction"
QT_MOC_LITERAL(8, 12), // "MissionAdded"
QT_MOC_LITERAL(21, 0), // ""
QT_MOC_LITERAL(22, 8), // "mission*"
QT_MOC_LITERAL(31, 1) // "m"

    },
    "faction\0MissionAdded\0\0mission*\0m"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_faction[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void faction::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<faction *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->MissionAdded((*reinterpret_cast< mission*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (faction::*)(mission * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&faction::MissionAdded)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject faction::staticMetaObject = { {
    QMetaObject::SuperData::link<ContainerObject::staticMetaObject>(),
    qt_meta_stringdata_faction.offsetsAndSize,
    qt_meta_data_faction,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_faction_t
, QtPrivate::TypeAndForceComplete<faction, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<mission *, std::false_type>



>,
    nullptr
} };


const QMetaObject *faction::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *faction::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_faction.stringdata0))
        return static_cast<void*>(this);
    return ContainerObject::qt_metacast(_clname);
}

int faction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContainerObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void faction::MissionAdded(mission * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_GlobalFactions_t {
    const uint offsetsAndSize[10];
    char stringdata0[40];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_GlobalFactions_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_GlobalFactions_t qt_meta_stringdata_GlobalFactions = {
    {
QT_MOC_LITERAL(0, 14), // "GlobalFactions"
QT_MOC_LITERAL(15, 12), // "MissionAdded"
QT_MOC_LITERAL(28, 0), // ""
QT_MOC_LITERAL(29, 8), // "mission*"
QT_MOC_LITERAL(38, 1) // "m"

    },
    "GlobalFactions\0MissionAdded\0\0mission*\0"
    "m"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GlobalFactions[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x08,    1 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void GlobalFactions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GlobalFactions *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->MissionAdded((*reinterpret_cast< mission*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject GlobalFactions::staticMetaObject = { {
    QMetaObject::SuperData::link<AdvancedContainer<faction>::staticMetaObject>(),
    qt_meta_stringdata_GlobalFactions.offsetsAndSize,
    qt_meta_data_GlobalFactions,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_GlobalFactions_t
, QtPrivate::TypeAndForceComplete<GlobalFactions, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<mission *, std::false_type>


>,
    nullptr
} };


const QMetaObject *GlobalFactions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GlobalFactions::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GlobalFactions.stringdata0))
        return static_cast<void*>(this);
    return AdvancedContainer<faction>::qt_metacast(_clname);
}

int GlobalFactions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AdvancedContainer<faction>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
