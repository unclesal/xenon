/****************************************************************************
** Meta object code from reading C++ file 'XeNetwork.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../common/network/XeNetwork.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'XeNetwork.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_xenon__XeNetwork_t {
    QByteArrayData data[21];
    char stringdata0[382];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_xenon__XeNetwork_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_xenon__XeNetwork_t qt_meta_stringdata_xenon__XeNetwork = {
    {
QT_MOC_LITERAL(0, 0, 16), // "xenon::XeNetwork"
QT_MOC_LITERAL(1, 17, 27), // "signal__got_autopilot_state"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 22), // "CommandAutopilotState&"
QT_MOC_LITERAL(4, 69, 9), // "autopilot"
QT_MOC_LITERAL(5, 79, 19), // "signal__got_ambient"
QT_MOC_LITERAL(6, 99, 7), // "Ambient"
QT_MOC_LITERAL(7, 107, 7), // "ambient"
QT_MOC_LITERAL(8, 115, 31), // "signal__got_user_aircraft_state"
QT_MOC_LITERAL(9, 147, 14), // "AircraftState&"
QT_MOC_LITERAL(10, 162, 13), // "user_aircraft"
QT_MOC_LITERAL(11, 176, 26), // "signal__got_bimbo_aircraft"
QT_MOC_LITERAL(12, 203, 14), // "bimbo_aircraft"
QT_MOC_LITERAL(13, 218, 24), // "signal__xplane_connected"
QT_MOC_LITERAL(14, 243, 27), // "signal__xplane_disconnected"
QT_MOC_LITERAL(15, 271, 15), // "slot__need_send"
QT_MOC_LITERAL(16, 287, 11), // "CommandSet&"
QT_MOC_LITERAL(17, 299, 3), // "cmd"
QT_MOC_LITERAL(18, 303, 24), // "slot__something_received"
QT_MOC_LITERAL(19, 328, 28), // "slot__disconnect_timer_fired"
QT_MOC_LITERAL(20, 357, 24) // "slot__reopen_timer_fired"

    },
    "xenon::XeNetwork\0signal__got_autopilot_state\0"
    "\0CommandAutopilotState&\0autopilot\0"
    "signal__got_ambient\0Ambient\0ambient\0"
    "signal__got_user_aircraft_state\0"
    "AircraftState&\0user_aircraft\0"
    "signal__got_bimbo_aircraft\0bimbo_aircraft\0"
    "signal__xplane_connected\0"
    "signal__xplane_disconnected\0slot__need_send\0"
    "CommandSet&\0cmd\0slot__something_received\0"
    "slot__disconnect_timer_fired\0"
    "slot__reopen_timer_fired"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_xenon__XeNetwork[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       5,    1,   67,    2, 0x06 /* Public */,
       8,    1,   70,    2, 0x06 /* Public */,
      11,    1,   73,    2, 0x06 /* Public */,
      13,    0,   76,    2, 0x06 /* Public */,
      14,    0,   77,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    1,   78,    2, 0x0a /* Public */,
      18,    0,   81,    2, 0x08 /* Private */,
      19,    0,   82,    2, 0x08 /* Private */,
      20,    0,   83,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   12,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void xenon::XeNetwork::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<XeNetwork *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signal__got_autopilot_state((*reinterpret_cast< CommandAutopilotState(*)>(_a[1]))); break;
        case 1: _t->signal__got_ambient((*reinterpret_cast< Ambient(*)>(_a[1]))); break;
        case 2: _t->signal__got_user_aircraft_state((*reinterpret_cast< AircraftState(*)>(_a[1]))); break;
        case 3: _t->signal__got_bimbo_aircraft((*reinterpret_cast< AircraftState(*)>(_a[1]))); break;
        case 4: _t->signal__xplane_connected(); break;
        case 5: _t->signal__xplane_disconnected(); break;
        case 6: _t->slot__need_send((*reinterpret_cast< CommandSet(*)>(_a[1]))); break;
        case 7: _t->slot__something_received(); break;
        case 8: _t->slot__disconnect_timer_fired(); break;
        case 9: _t->slot__reopen_timer_fired(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (XeNetwork::*)(CommandAutopilotState & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XeNetwork::signal__got_autopilot_state)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (XeNetwork::*)(Ambient );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XeNetwork::signal__got_ambient)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (XeNetwork::*)(AircraftState & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XeNetwork::signal__got_user_aircraft_state)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (XeNetwork::*)(AircraftState & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XeNetwork::signal__got_bimbo_aircraft)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (XeNetwork::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XeNetwork::signal__xplane_connected)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (XeNetwork::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&XeNetwork::signal__xplane_disconnected)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject xenon::XeNetwork::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_xenon__XeNetwork.data,
    qt_meta_data_xenon__XeNetwork,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *xenon::XeNetwork::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *xenon::XeNetwork::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_xenon__XeNetwork.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int xenon::XeNetwork::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void xenon::XeNetwork::signal__got_autopilot_state(CommandAutopilotState & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void xenon::XeNetwork::signal__got_ambient(Ambient _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void xenon::XeNetwork::signal__got_user_aircraft_state(AircraftState & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void xenon::XeNetwork::signal__got_bimbo_aircraft(AircraftState & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void xenon::XeNetwork::signal__xplane_connected()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void xenon::XeNetwork::signal__xplane_disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
